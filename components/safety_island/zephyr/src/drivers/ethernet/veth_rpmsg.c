/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zephyr/drivers/mbox.h>
#include <zephyr/net/ethernet.h>
#include <zephyr/net/net_if.h>

#include <openamp/open_amp.h>
#include <metal/device.h>
#include <resource_table.h>

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(veth_rpmsg, LOG_LEVEL_INF);

#if !DT_HAS_CHOSEN(zephyr_rpmsg_shm) || !DT_HAS_CHOSEN(zephyr_rpmsg_rsc_table)\
	|| !DT_HAS_CHOSEN(zephyr_rpmsg_mbox_dev)
#error "Sample requires definition of shared memory, rsc table and mbox dev"
#endif

#define MBOX_DEV_NODE   DT_NODELABEL(pc_mbox_rpmsg_binding)
#define RSC_TABLE_NODE  DT_CHOSEN(zephyr_rpmsg_rsc_table)
#define SHM_NODE        DT_CHOSEN(zephyr_rpmsg_shm)
#define VETH_RPMSG_DEVICE_NAME "veth_rpmsg"
#define VETH_RPMSG_MEM_REGION_NUM 2

#define RPMSG_DETACH_CH_ID  2
#define RPMSG_ATTACH_CH_ID  3
#define RPMSG_ACK_MBX_ID    6
#define RPMSG_MBX_MAX       7
#define RPMSG_MBX_TX_ID     1
#define RPMSG_MAX_EVENTS_VQ 2

#define MAC_LENGTH 6
const static uint8_t default_mac[MAC_LENGTH] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

struct veth_rpmsg_mbox {
	struct mbox_channel channel;
	int (*mbox_cb)(const struct device *dev, uint32_t channel,
		       void *user_data, struct mbox_msg *msg);
};

struct veth_rpmsg_ctx {
	struct k_work notify_work;

	struct net_if *iface;
	uint8_t mac[MAC_LENGTH];

	struct metal_io_region *rsc_io;
	struct metal_io_region *shm_io;
	struct rpmsg_virtio_shm_pool shpool;
	metal_phys_addr_t shm_physmap;

	struct metal_device shm_device;

	struct veth_rpmsg_mbox mbox[RPMSG_MBX_MAX];

	unsigned int rpmsg_state;
	const struct veth_rpmsg_conf *cfg;
	struct rpmsg_virtio_device rvdev;
	struct rpmsg_device *rpdev;
	struct rpmsg_endpoint sc_ept;
};

struct veth_rpmsg_conf {
	char *device_name;
	void *rsc_table_addr;
	int rsc_table_size;
	void *shm_addr;
	int shm_size;
	long shm_bus_addr_delta_h;
	long shm_bus_addr_delta_l;
};

static const struct veth_rpmsg_conf veth_rpmsg_config = {
	.device_name = VETH_RPMSG_DEVICE_NAME,
	.rsc_table_addr = DT_REG_ADDR(RSC_TABLE_NODE),
	.rsc_table_size = DT_REG_SIZE(RSC_TABLE_NODE),
	.shm_addr = DT_REG_ADDR(SHM_NODE),
	.shm_size = DT_REG_SIZE(SHM_NODE),
	.shm_bus_addr_delta_h = DT_PROP(SHM_NODE, busaddr_delta_h),
	.shm_bus_addr_delta_l = DT_PROP(SHM_NODE, busaddr_delta_l),
};

enum rproc_state {
	RPMSG_ATTACH = 0,
	RPMSG_DETACH = 1,
};

static struct veth_rpmsg_ctx veth_rpmsg_contex;

bool rpmsg_ready;

static int mailbox_notify(void *priv, uint32_t vqid)
{
	struct veth_rpmsg_ctx *ctx = priv;
	unsigned int i = vqid * RPMSG_MAX_EVENTS_VQ + RPMSG_MBX_TX_ID;

	__ASSERT_NO_MSG(i < RPMSG_MBX_MAX);

	return mbox_send(&ctx->mbox[i].channel, NULL);
}

static struct net_if *get_iface(struct veth_rpmsg_ctx *ctx, uint16_t vlan_tag)
{
#if defined(CONFIG_NET_VLAN)
	struct net_if *iface;

	iface = net_eth_get_vlan_iface(ctx->iface, vlan_tag);
	if (!iface) {
		return ctx->iface;
	}

	return iface;
#else
	ARG_UNUSED(vlan_tag);

	return ctx->iface;
#endif
}

static int rpmsg_recv_callback(struct rpmsg_endpoint *ept, void *data,
			       size_t len, uint32_t src, void *priv)
{
	uint16_t vlan_tag = NET_VLAN_TAG_UNSPEC;
	struct veth_rpmsg_ctx *ctx = priv;
	struct net_if *iface = ctx->iface;
	struct net_pkt *pkt = NULL;
	int ret;

	if (data) {
		if (!rpmsg_ready) {
			rpmsg_ready = true;
			return 0;
		}

		pkt = net_pkt_rx_alloc_with_buffer(iface, len, AF_UNSPEC, 0,
						   K_FOREVER);
		if (!pkt) {
			LOG_ERR("Failed to allocate packet.\n");
			return -1;
		}

		ret = net_pkt_write(pkt, data, len);
		if (ret != 0) {
			LOG_ERR("Failed to write buff into packet. (%d)\n",
				ret);
			net_pkt_unref(pkt);
			return -1;
		}

#if defined(CONFIG_NET_VLAN)
		struct net_eth_hdr *hdr = NET_ETH_HDR(pkt);

		if (ntohs(hdr->type) == NET_ETH_PTYPE_VLAN) {
			struct net_eth_vlan_hdr *hdr_vlan =
				(struct net_eth_vlan_hdr *)
				NET_ETH_HDR(pkt);

			net_pkt_set_vlan_tci(pkt, ntohs(hdr_vlan->vlan.tci));
			vlan_tag = net_pkt_vlan_tag(pkt);

#if CONFIG_NET_TC_RX_COUNT > 1
			enum net_priority prio;

			prio = net_vlan2priority(net_pkt_vlan_priority(pkt));
			net_pkt_set_priority(pkt, prio);
#endif
		}
#endif /* CONFIG_NET_VLAN */

		ret = net_recv_data(get_iface(ctx, vlan_tag), pkt);
		if (ret < 0) {
			LOG_ERR("Failed to write data into Rx Q: %d.\n", ret);
			net_pkt_unref(pkt);
			return -1;
		}
	} else {
		LOG_ERR("Data is invalid.\n");
		return -1;
	}

	return 0;
}

static void veth_rpmsg_destroy_vdev(struct veth_rpmsg_ctx *ctx)
{
	rpmsg_deinit_vdev(&ctx->rvdev);
	rproc_virtio_remove_vdev(ctx->rvdev.vdev);
	net_arp_clear_cache(ctx->iface);
}

static int veth_rpmsg_setup_vdev(struct veth_rpmsg_ctx *ctx)
{
	int ret;
	struct fw_rsc_vdev_vring *vring_rsc;
	struct virtio_device *vdev;
	const struct veth_rpmsg_conf *cfg = ctx->cfg;

	vdev = rproc_virtio_create_vdev(VIRTIO_DEV_DEVICE, VDEV_ID,
					rsc_table_to_vdev(cfg->rsc_table_addr),
					ctx->rsc_io, ctx, mailbox_notify, NULL);

	if (!vdev) {
		LOG_ERR("Failed to create vdev.\n");
		return -1;
	}

	vring_rsc = rsc_table_get_vring0(cfg->rsc_table_addr);
	ret = rproc_virtio_init_vring(vdev, 0, vring_rsc->notifyid,
				      (void *)vring_rsc->da, ctx->rsc_io,
				      vring_rsc->num, vring_rsc->align);
	if (ret) {
		LOG_ERR("Failed to init vring 0.\n");
		goto failed;
	}

	vring_rsc = rsc_table_get_vring1(cfg->rsc_table_addr);
	ret = rproc_virtio_init_vring(vdev, 1, vring_rsc->notifyid,
				      (void *)vring_rsc->da, ctx->rsc_io,
				      vring_rsc->num, vring_rsc->align);
	if (ret) {
		LOG_ERR("Failed to init vring 1.\n");
		goto failed;
	}

	rpmsg_virtio_init_shm_pool(&ctx->shpool, NULL, cfg->shm_size);
	ret = rpmsg_init_vdev(&ctx->rvdev, vdev, NULL, ctx->shm_io,
			      &ctx->shpool);
	if (ret) {
		LOG_ERR("Failed to init vdev.\n");
		goto failed;
	}

	ctx->rpdev = rpmsg_virtio_get_rpmsg_device(&ctx->rvdev);
	if (!ctx->rpdev) {
		LOG_ERR("Failed to create rpmsg virtio device.\n");
		goto failed;
	}

	ret = rpmsg_create_ept(&ctx->sc_ept, ctx->rpdev, CONFIG_VETH_NS_NAME,
			       RPMSG_ADDR_ANY, RPMSG_ADDR_ANY,
			       rpmsg_recv_callback, NULL);

	if (ret) {
		LOG_ERR("Failed to create rpmsg endpoint.\n");
		goto failed;
	}

	return 0;

failed:
	rproc_virtio_remove_vdev(vdev);

	return -1;
}

static void
platform_mbox_callback(const struct device *dev, uint32_t id,
		       void *context, struct mbox_msg *msg)
{
	int ret;
	struct veth_rpmsg_ctx *ctx = context;

	switch (ctx->rpmsg_state) {
	case RPMSG_ATTACH:
		if (id == RPMSG_DETACH_CH_ID || id == RPMSG_ATTACH_CH_ID) {
			veth_rpmsg_destroy_vdev(ctx);
			ctx->rpmsg_state = RPMSG_DETACH;
			rpmsg_ready = false;
			if (id == RPMSG_ATTACH_CH_ID)
				mbox_send(&ctx->mbox[RPMSG_ACK_MBX_ID].channel,
					  NULL);
			LOG_INF("RPMSG Endpoint: DETACHED\n");
		} else {
			(void)k_work_submit(&ctx->notify_work);
		}
		break;
	case RPMSG_DETACH:
		if (id == RPMSG_ATTACH_CH_ID) {
			mbox_send(&ctx->mbox[RPMSG_ACK_MBX_ID].channel, NULL);
		} else {
			ret = veth_rpmsg_setup_vdev(ctx);
			if (ret) {
				LOG_ERR("Failed to create rpmsg virtio device.\n");
			} else {
				ctx->rpmsg_state = RPMSG_ATTACH;
				LOG_INF("RPMSG Endpoint: ATTACHED\n");
			}
		}
		break;
	default:
		LOG_INF("RPMSG Endpoint: UNKNOWN\n");
		return;
	}
}

static void notify_handler(struct k_work *work)
{
	struct veth_rpmsg_ctx *ctx = CONTAINER_OF(work, struct veth_rpmsg_ctx, notify_work);
	int ret;

	ret = rproc_virtio_notified(ctx->rvdev.vdev, VRING1_ID);
	if (ret)
		LOG_ERR("Failed to notify rproc virtio: %d\n", ret);

}

void load_rsc_table(struct fw_resource_table *rsc_tab_addr, int rsc_tab_size)
{
	void *src_rsc_tab_addr;
	int src_rsc_tab_size;

	rsc_table_get(&src_rsc_tab_addr, &src_rsc_tab_size);

	if (src_rsc_tab_size > rsc_tab_size) {
		LOG_ERR("Warning Resource table is too large.\n");
		return;
	}

	memcpy(rsc_tab_addr, src_rsc_tab_addr, src_rsc_tab_size);
}

const struct veth_rpmsg_mbox veth_rpmsg_mbox[RPMSG_MBX_MAX] = {
	{
	    .channel = MBOX_DT_CHANNEL_GET(MBOX_DEV_NODE, vq0_rx),
	    .mbox_cb = platform_mbox_callback,
	},
	{
	    .channel = MBOX_DT_CHANNEL_GET(MBOX_DEV_NODE, vq0_tx),
	},
	{
	    .channel = MBOX_DT_CHANNEL_GET(MBOX_DEV_NODE, vq1_rx),
	    .mbox_cb = platform_mbox_callback,
	},
	{
	    .channel = MBOX_DT_CHANNEL_GET(MBOX_DEV_NODE, vq1_tx),
	},
	{
	    .channel = MBOX_DT_CHANNEL_GET(MBOX_DEV_NODE, detach),
	    .mbox_cb = platform_mbox_callback,
	},
	{
	    .channel = MBOX_DT_CHANNEL_GET(MBOX_DEV_NODE, attach),
	    .mbox_cb = platform_mbox_callback,
	},
	{
	    .channel = MBOX_DT_CHANNEL_GET(MBOX_DEV_NODE, ack),
	}
};

static int veth_rpmsg_mbox_init(struct veth_rpmsg_ctx *ctx)
{
	struct veth_rpmsg_mbox *mbox;
	uint32_t i;
	int ret;

	/* Initialise mailbox structure table */
	memcpy(ctx->mbox, veth_rpmsg_mbox, sizeof(veth_rpmsg_mbox));

	for (i = 0; i < RPMSG_MBX_MAX; i++) {
		mbox = &ctx->mbox[i];

		ret = mbox_set_enabled(&mbox->channel, 1);
		if (ret) {
			LOG_ERR("Failed to enable rx mbox device.\n");
			return ret;
		}

		if (mbox->mbox_cb) {
			ret = mbox_register_callback(&mbox->channel,
						     mbox->mbox_cb, ctx);
			if (ret) {
				LOG_ERR("Failed to register mbox callback.\n");
				return ret;
			}
		}
	}

	return 0;
}

int veth_rpmsg_platform_init(struct veth_rpmsg_conf *cfg, struct veth_rpmsg_ctx *ctx)
{
	struct metal_device *device;
	struct metal_init_params metal_params = METAL_INIT_DEFAULTS;
	int ret;

	ret = metal_init(&metal_params);
	if (ret) {
		LOG_ERR("Failed to initialize metal: %d\n", ret);
		return -1;
	}

	ctx->shm_device.name = cfg->device_name;
	ctx->shm_device.num_regions = VETH_RPMSG_MEM_REGION_NUM;
	ret = metal_register_generic_device(&ctx->shm_device);
	if (ret) {
		LOG_ERR("Couldn't register shared memory: %d\n", ret);
		return -1;
	}

	ret = metal_device_open("generic", ctx->shm_device.name, &device);
	if (ret) {
		LOG_ERR("Failed to open shm device: %d\n", ret);
		return -1;
	}

	ctx->shm_physmap = (metal_phys_addr_t)cfg->shm_addr +
			   (metal_phys_addr_t)cfg->shm_bus_addr_delta_l +
			   (metal_phys_addr_t)(cfg->shm_bus_addr_delta_h << 32);
	/* declare shared memory region */
	metal_io_init(&device->regions[0], (void *)cfg->shm_addr, &ctx->shm_physmap,
		      cfg->shm_size, -1, 0, NULL);

	ctx->shm_io = metal_device_io_region(device, 0);
	if (!ctx->shm_io) {
		LOG_ERR("Failed to get shm_io region.\n");
		return -1;
	}

	/* declare resource table region */
	load_rsc_table(cfg->rsc_table_addr, cfg->rsc_table_size);

	metal_io_init(&device->regions[1], cfg->rsc_table_addr,
		      (metal_phys_addr_t *)cfg->rsc_table_addr,
		      cfg->rsc_table_size, -1, 0, NULL);

	ctx->rsc_io = metal_device_io_region(device, 1);
	if (!ctx->rsc_io) {
		LOG_ERR("Failed to get rsc_io region\n");
		return -1;
	}

	k_work_init(&ctx->notify_work, notify_handler);

	ret = veth_rpmsg_mbox_init(ctx);
	if (ret) {
		LOG_ERR("Failed to init mbox\n");
		return -1;
	}

	return 0;
}

int veth_rpmsg_init(const struct device *dev)
{
	struct veth_rpmsg_ctx *ctx = dev->data;

	memset(ctx, 0, sizeof(struct veth_rpmsg_ctx));

	if (CONFIG_VETH_MAC_ADDR[0] != 0) {
		if (net_bytes_from_str(ctx->mac, sizeof(ctx->mac),
				       CONFIG_VETH_MAC_ADDR) == 0) {
			return 0;
		}
	}

	memcpy(ctx->mac, default_mac, MAC_LENGTH);
	return 0;
}

static void veth_rpmsg_iface_init(struct net_if *iface)
{
	const struct device *dev = net_if_get_device(iface);
	struct veth_rpmsg_ctx *ctx = dev->data;
	const struct veth_rpmsg_conf *cfg = dev->config;
	int ret;

	ret = veth_rpmsg_platform_init(cfg, ctx);
	if (ret) {
		LOG_ERR("Failed to initialize platform.\n");
		return;
	}

	ctx->iface = iface;
	ethernet_init(iface);

	if (net_if_set_link_addr(iface, ctx->mac, MAC_LENGTH, NET_LINK_ETHERNET) < 0) {
		LOG_ERR("Failed to set link address.\n");
		return;
	}

}

static enum ethernet_hw_caps veth_rpmsg_caps(const struct device *dev)
{
	return (0
#if defined(CONFIG_NET_VLAN)
		| ETHERNET_HW_VLAN
#endif
#if defined(CONFIG_NET_PROMISCUOUS_MODE)
		| ETHERNET_PROMISC_MODE
#endif
	);
}

static int veth_rpmsg_set_config(const struct device *dev,
				 enum ethernet_config_type type,
				 const struct ethernet_config *config)
{
	int ret = 0;

	(void) dev;
	(void) type;
	(void) config;

	switch (type) {
#if defined(CONFIG_NET_PROMISCUOUS_MODE)
	case ETHERNET_CONFIG_TYPE_PROMISC_MODE:
		/* Always succeed */
		break;
#endif

	default:
		ret = -ENOTSUP;
		break;
	}

	return ret;
}

int veth_rpmsg_send(const struct device *dev, struct net_pkt *pkt)
{
	struct veth_rpmsg_ctx *ctx = dev->data;
	uint8_t frame_buf[NET_ETH_MAX_FRAME_SIZE];
	size_t packet_length = net_pkt_get_len(pkt);
	int ret;

	if (!rpmsg_ready)
		return -EIO;

	if (net_pkt_read(pkt, frame_buf, packet_length)) {
		LOG_ERR("Packet read failed.\n");
		return -EIO;
	}

	ret = rpmsg_send(&ctx->sc_ept, frame_buf, packet_length);
	if (ret < 0) {
		LOG_DBG("Rpmsg endpoint not ready or sending failed.\n");
		return -EIO;
	}

	return 0;
}

static int veth_rpmsg_start(const struct device *dev)
{
	struct veth_rpmsg_ctx *ctx = dev->data;
	const struct veth_rpmsg_conf *cfg = dev->config;

	ctx->rpmsg_state = RPMSG_DETACH;
	ctx->sc_ept.priv = ctx;
	ctx->cfg = cfg;

	return 0;
}

static const struct ethernet_api veth_rpmsg_api = {
	.iface_api.init     = veth_rpmsg_iface_init,
	.start              = veth_rpmsg_start,
	.get_capabilities   = veth_rpmsg_caps,
	.set_config         = veth_rpmsg_set_config,
	.send               = veth_rpmsg_send,
};

ETH_NET_DEVICE_INIT(veth_rpmsg,
		    "veth_rpmsg",
		    veth_rpmsg_init, NULL,
		    &veth_rpmsg_contex,
		    &veth_rpmsg_config,
		    CONFIG_ETH_INIT_PRIORITY,
		    &veth_rpmsg_api,
		    NET_ETH_MTU);

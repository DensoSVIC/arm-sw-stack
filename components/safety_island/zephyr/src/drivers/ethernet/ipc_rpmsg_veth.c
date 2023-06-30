/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_MODULE_NAME ipc_rpmsg_veth
#define LOG_LEVEL CONFIG_ETHERNET_LOG_LEVEL

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

#include <stdio.h>

#include <zephyr/kernel.h>
#include <stdbool.h>
#include <errno.h>
#include <stddef.h>

#include <zephyr/net/net_pkt.h>
#include <zephyr/net/net_core.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/ethernet.h>
#include <ethernet/eth_stats.h>

#include <zephyr/ipc/ipc_service.h>

#define NET_BUF_TIMEOUT K_MSEC(100)

struct ipc_rpmsg_veth_context {
	const struct device *ipc_instance;
	const char *if_name, *ipv4_addr, *netmask, *if_mac_addr;
	uint8_t send[NET_ETH_MAX_FRAME_SIZE];
	uint8_t mac_addr[6];
	bool init_done;
	bool ep_bound;
	struct ipc_ept ipc_ept;
	struct ipc_ept_cfg ipc_ept_cfg;
	struct net_linkaddr ll_addr;
	struct net_if *iface;
#if defined(CONFIG_NET_STATISTICS_ETHERNET)
	struct net_stats_eth stats;
#endif
};

static void ipc_rpmsg_veth_ept_bound(void *priv)
{
	struct ipc_rpmsg_veth_context *ctx = priv;

	ctx->ep_bound = true;
	LOG_DBG("Endpoint \"%s\" connected.", ctx->if_name);
}

static void ipc_rpmsg_veth_ept_recv(const void *data, size_t len, void *priv)
{
	struct ipc_rpmsg_veth_context *ctx = priv;
	struct net_if *iface = ctx->iface;
	struct net_pkt *pkt;

	if (net_if_is_up(ctx->iface) && ctx->ep_bound) {
		pkt = net_pkt_rx_alloc_with_buffer(ctx->iface, len, AF_UNSPEC,
						   0, K_FOREVER);
		if (!pkt) {
			LOG_ERR("Failed to allocate packet.");
			return;
		}

		if (net_pkt_write(pkt, data, len)) {
			LOG_ERR("Failed to write buff into packet.");
			net_pkt_unref(pkt);
			return;
		}

		if (net_recv_data(iface, pkt) < 0) {
			LOG_ERR("Failed to write data into Rx.");
			net_pkt_unref(pkt);
			return;
		}

		LOG_DBG("%s:Recv pkt %p len %ld", ctx->if_name, pkt, len);
	}
}

#if defined(CONFIG_NET_STATISTICS_ETHERNET)
static struct net_stats_eth *ipc_rpmsg_veth_get_stats(const struct device *dev)
{
	struct ipc_rpmsg_veth_context *ctx = dev->data;

	return &ctx->stats;
}
#endif

static struct net_linkaddr *
ipc_rpmsg_veth_get_mac(struct ipc_rpmsg_veth_context *ctx)
{
	ctx->ll_addr.addr = ctx->mac_addr;
	ctx->ll_addr.len = sizeof(ctx->mac_addr);

	return &ctx->ll_addr;
}

static void ipc_rpmsg_veth_iface_init(struct net_if *iface)
{
	struct ipc_rpmsg_veth_context *ctx = net_if_get_device(iface)->data;
	struct net_linkaddr *ll_addr;
	struct in_addr addr;

	if (!ctx->iface) {
		ctx->iface = iface;
	}

	ethernet_init(iface);

	if (ctx->init_done) {
		return;
	}

	ctx->init_done = true;

	LOG_DBG("Interface %p using \"%s\"", iface, ctx->if_name);

	if (net_bytes_from_str(ctx->mac_addr, sizeof(ctx->mac_addr),
			       ctx->if_mac_addr) < 0) {
		LOG_ERR("Invalid MAC address %s", ctx->if_mac_addr);
	} else {
		ll_addr = ipc_rpmsg_veth_get_mac(ctx);
		net_if_set_link_addr(iface, ll_addr->addr,
				     ll_addr->len, NET_LINK_ETHERNET);
	}

	if (net_addr_pton(AF_INET, ctx->ipv4_addr, &addr)) {
		LOG_ERR("Invalid IPv4 address: %s", ctx->ipv4_addr);
	} else {
		net_if_ipv4_addr_add(iface, &addr, NET_ADDR_MANUAL, 0);
	}

	if (net_addr_pton(AF_INET, ctx->netmask, &addr)) {
		LOG_ERR("Invalid netmask: %s", ctx->netmask);
	} else {
		net_if_ipv4_set_netmask(iface, &addr);
	}
}

static enum ethernet_hw_caps ipc_rpmsg_veth_caps(const struct device *dev)
{
	ARG_UNUSED(dev);

	return (0
#if defined(CONFIG_NET_PROMISCUOUS_MODE)
		| ETHERNET_PROMISC_MODE
#endif
	);
}

static int ipc_rpmsg_veth_set_config(const struct device *dev,
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

int ipc_rpmsg_veth_send(const struct device *dev, struct net_pkt *pkt)
{
	struct ipc_rpmsg_veth_context *ctx = dev->data;
	int count = net_pkt_get_len(pkt);
	int ret;

	if (net_if_is_up(ctx->iface) && ctx->ep_bound) {
		ret = net_pkt_read(pkt, ctx->send, count);
		if (ret) {
			LOG_ERR("failed to read pkt ret %d", ret);
			return ret;
		}

		LOG_DBG("%s:Send pkt %p len %d", ctx->if_name, pkt, count);

		ret = ipc_service_send(&ctx->ipc_ept, ctx->send, count);
		if (ret < 0) {
			LOG_ERR("send failed with ret %d", ret);
			return ret;
		}
	}

	return 0;
}

static int ipc_rpmsg_veth_start(const struct device *dev)
{
	struct ipc_rpmsg_veth_context *ctx = dev->data;
	int ret;

	if (ctx->ep_bound) {
		return 0;
	}

	ret = ipc_service_open_instance(ctx->ipc_instance);
	if (ret < 0 && ret != -EALREADY) {
		LOG_ERR("ipc_service_open_instance() failure :%d", ret);
		return -EINVAL;
	}

	ctx->ipc_ept_cfg.priv = ctx;
	ret = ipc_service_register_endpoint(ctx->ipc_instance,
					    &ctx->ipc_ept,
					    &ctx->ipc_ept_cfg);
	if (ret < 0) {
		LOG_ERR("ipc_service_register_endpoint() failure :%d", ret);
		return -EINVAL;
	}

	return ret;
}

int ipc_rpmsg_veth_probe(const struct device *dev)
{
	ARG_UNUSED(dev);

	return 0;
}

static const struct ethernet_api ipc_rpmsg_veth_api = {
	.iface_api.init   = ipc_rpmsg_veth_iface_init,
	.start            = ipc_rpmsg_veth_start,
	.get_capabilities = ipc_rpmsg_veth_caps,
	.set_config       = ipc_rpmsg_veth_set_config,
	.send             = ipc_rpmsg_veth_send,
#if defined(CONFIG_NET_STATISTICS_ETHERNET)
	.get_stats        = ipc_rpmsg_veth_get_stats,
#endif
};

#define DEFINE_IPC_RPMSG_VETH_DEV_DATA(x, _)                                  \
	static struct ipc_rpmsg_veth_context                                  \
		ipc_rpmsg_veth_context_data_##x = {                           \
			.ipc_instance =                                       \
			    DEVICE_DT_GET(DT_NODELABEL(ipc_rpmsg_veth_##x)),  \
			.if_name = CONFIG_IPC_RPMSG_VETH_DRV_NAME #x,         \
			.if_mac_addr = CONFIG_IPC_RPMSG_VETH_MAC_ADDR_##x,    \
			.ipv4_addr = CONFIG_IPC_RPMSG_VETH_SI_IPV4_ADDR_##x,  \
			.netmask = CONFIG_IPC_RPMSG_VETH_SI_NETMASK_##x,      \
			.ipc_ept_cfg.name = CONFIG_IPC_RPMSG_VETH_EPT_NAME_##x,\
			.ipc_ept_cfg.cb = {                                   \
			     .bound    = ipc_rpmsg_veth_ept_bound,            \
			     .received = ipc_rpmsg_veth_ept_recv,             \
			 },                                                   \
	}
LISTIFY(CONFIG_IPC_RPMSG_VETH_COUNT, DEFINE_IPC_RPMSG_VETH_DEV_DATA, (;), _);

#define DEFINE_IPC_RPMSG_VETH_DEVICE(x, _)                               \
	ETH_NET_DEVICE_INIT(veth_rpmsg_ipc_##x,                          \
			    CONFIG_IPC_RPMSG_VETH_DRV_NAME #x,           \
			    ipc_rpmsg_veth_probe, NULL,                  \
			    &ipc_rpmsg_veth_context_data_##x,            \
			    NULL,                                        \
			    CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,         \
			    &ipc_rpmsg_veth_api,                         \
			    NET_ETH_MTU)

LISTIFY(CONFIG_IPC_RPMSG_VETH_COUNT, DEFINE_IPC_RPMSG_VETH_DEVICE, (;), _);

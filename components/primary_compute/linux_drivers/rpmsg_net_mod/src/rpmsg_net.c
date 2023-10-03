/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Rpmsg net driver
 *
 * Yanqin Wei <Yanqin.wei@arm.com>
 * Luca Fancellu <luca.fancellu@arm.com>
 */

#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/if_vlan.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/rpmsg.h>

struct rpmsg_ept_netdev {
	struct net_device *dev;
	struct rpmsg_device *rpdev;
};

static const char ack_rpmsg = 0;

static int rpmsg_netdev_cb(struct rpmsg_device *rpdev, void *data, int len,
			   void *priv, u32 src)
{
	struct rpmsg_ept_netdev *eptdev = dev_get_drvdata(&rpdev->dev);
	struct sk_buff *skb;

	skb = alloc_skb(len, GFP_ATOMIC);
	if (!skb)
		return -ENOMEM;

	skb_put_data(skb, data, len);

	skb->protocol = eth_type_trans(skb, eptdev->dev);
	eptdev->dev->stats.rx_packets++;
	eptdev->dev->stats.rx_bytes += len;
	netif_rx(skb);

	return 0;
}

static int rpmsg_net_open(struct net_device *dev)
{
	struct rpmsg_ept_netdev *eptdev = netdev_priv(dev);
	struct rpmsg_device *rpdev = eptdev->rpdev;
	struct rpmsg_channel_info chinfo = {
		.src = rpdev->src,
		.dst = RPMSG_ADDR_ANY,
	};
	ssize_t rpmsg_mtu;
	int ret;

	strncpy(chinfo.name, rpdev->id.name, RPMSG_NAME_SIZE);
	rpdev->ept = rpmsg_create_ept(rpdev, rpmsg_netdev_cb, NULL, chinfo);
	if (!rpdev->ept)
		return -ENOMEM;

	rpdev->src = rpdev->ept->addr;
	rpmsg_mtu = rpmsg_get_mtu(rpdev->ept);
	if (rpmsg_mtu < 0) {
		dev_err(&rpdev->dev, "Invalid mtu\n");
		ret = rpmsg_mtu;
		goto err;
	}
	dev->min_mtu = ETH_MIN_MTU;
	dev->max_mtu = rpmsg_mtu - (ETH_HLEN + VLAN_HLEN);
	ret = rpmsg_trysend(rpdev->ept, (void *)&ack_rpmsg, sizeof(ack_rpmsg));
	if (ret) {
		dev_err(&rpdev->dev, "Sending ack rpmsg failed\n");
		goto err;
	}

	return 0;
err:
	rpmsg_destroy_ept(rpdev->ept);
	rpdev->ept = NULL;
	return ret;
}

static int rpmsg_net_stop(struct net_device *dev)
{
	struct rpmsg_ept_netdev *eptdev = netdev_priv(dev);

	rpmsg_destroy_ept(eptdev->rpdev->ept);
	eptdev->rpdev->ept = NULL;

	return 0;
}

static netdev_tx_t rpmsg_net_start_xmit(struct sk_buff *skb,
					struct net_device *dev)
{
	struct rpmsg_ept_netdev *eptdev = netdev_priv(dev);
	int ret;

	if (eptdev == NULL) {
		dev_err(&dev->dev, "Invalid eptdev.\n");
		return NETDEV_TX_BUSY;
	}

	skb_tx_timestamp(skb);

	ret = rpmsg_trysend(eptdev->rpdev->ept, (void *)skb->data, skb->len);

	if (ret) {
		dev->stats.tx_errors++;
		dev_dbg(&dev->dev, "Rpmsg sending failed, ret = %d.\n", ret);
		return NETDEV_TX_BUSY;
	} else {
		dev->stats.tx_packets++;
		dev->stats.tx_bytes += skb->len;
	}

	dev_kfree_skb(skb);

	return NETDEV_TX_OK;
}

static const struct ethtool_ops rpmsg_net_ethtool_ops = {
	.get_ts_info = ethtool_op_get_ts_info,
};

static const struct net_device_ops rpmsg_netdev_ops = {
	.ndo_open         = rpmsg_net_open,
	.ndo_stop         = rpmsg_net_stop,
	.ndo_start_xmit   = rpmsg_net_start_xmit,
};

static int rpmsg_netdev_probe(struct rpmsg_device *rpdev)
{
	struct rpmsg_ept_netdev *rpmsg_ept;
	struct net_device *netdev;
	int ret;

	/* only support one mq */
	netdev = alloc_etherdev_mq(sizeof(struct rpmsg_ept_netdev), 1);
	if (!netdev)
		return -ENOMEM;

	/* Set up network device as normal. */
	netdev->priv_flags |= IFF_UNICAST_FLT | IFF_LIVE_ADDR_CHANGE;
	netdev->netdev_ops = &rpmsg_netdev_ops;
	netdev->ethtool_ops = &rpmsg_net_ethtool_ops;
	netdev->features = NETIF_F_HIGHDMA;

	netdev->vlan_features = netdev->features;

	snprintf(netdev->name, IFNAMSIZ, "%s", rpdev->id.name);

	eth_hw_addr_random(netdev);

	/* Set up our device-specific information */
	rpmsg_ept = netdev_priv(netdev);
	rpmsg_ept->rpdev = rpdev;
	rpmsg_ept->dev = netdev;
	dev_set_drvdata(&rpdev->dev, rpmsg_ept);

	netif_set_real_num_tx_queues(netdev, 1);
	netif_set_real_num_rx_queues(netdev, 1);

	ret = register_netdev(netdev);
	if (ret) {
		dev_err(&rpdev->dev, "registering net device failed\n");
		free_netdev(rpmsg_ept->dev);
		return ret;
	}

	return ret;
}

static void rpmsg_netdev_remove(struct rpmsg_device *rpdev)
{
	struct rpmsg_ept_netdev *eptdev = dev_get_drvdata(&rpdev->dev);

	unregister_netdev(eptdev->dev);
	free_netdev(eptdev->dev);
}

static const struct rpmsg_device_id rpmsg_netdev_id_table[] = {
	{ .name = "ethsi0" },
	{ .name = "ethsi1" },
	{ .name = "ethsi2" },
	{ },
};
MODULE_DEVICE_TABLE(rpmsg, rpmsg_netdev_id_table);

static struct rpmsg_driver rpmsg_netdev_driver = {
	.probe = rpmsg_netdev_probe,
	.remove = rpmsg_netdev_remove,
	.id_table   = rpmsg_netdev_id_table,
	.drv = {
		.name = "rpmsg_netdev",
	},
};
module_rpmsg_driver(rpmsg_netdev_driver);

MODULE_ALIAS("rpmsg:rpmsg_netdev");
MODULE_LICENSE("GPL");

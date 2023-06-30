/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/net/ethernet.h>
#include <zephyr/net/ethernet_bridge.h>
#include <zephyr/net/net_if.h>

#include <stdio.h>

struct if_desc {
	const char *addr;
	const char *gw;
	const char *netmask;
	uint16_t vlan;
	bool bridge;
};

static const struct if_desc ifs[] = {
	{
		.addr    = CONFIG_NET_IFACE1_ADDR,
		.gw      = CONFIG_NET_IFACE1_GW,
		.netmask = CONFIG_NET_IFACE1_NETMASK,
		.vlan    = CONFIG_NET_IFACE1_VLAN,
		.bridge  = IS_ENABLED(CONFIG_NET_IFACE1_BRIDGE)
	},
	{
		.addr    = CONFIG_NET_IFACE2_ADDR,
		.gw      = CONFIG_NET_IFACE2_GW,
		.netmask = CONFIG_NET_IFACE2_NETMASK,
		.vlan    = CONFIG_NET_IFACE2_VLAN,
		.bridge  = IS_ENABLED(CONFIG_NET_IFACE2_BRIDGE)
	},
	{
		.addr    = CONFIG_NET_IFACE3_ADDR,
		.gw      = CONFIG_NET_IFACE3_GW,
		.netmask = CONFIG_NET_IFACE3_NETMASK,
		.vlan    = CONFIG_NET_IFACE3_VLAN,
		.bridge  = IS_ENABLED(CONFIG_NET_IFACE3_BRIDGE)
	},
	{
		.addr    = CONFIG_NET_IFACE4_ADDR,
		.gw      = CONFIG_NET_IFACE4_GW,
		.netmask = CONFIG_NET_IFACE4_NETMASK,
		.vlan    = CONFIG_NET_IFACE4_VLAN,
		.bridge  = IS_ENABLED(CONFIG_NET_IFACE4_BRIDGE)
	}
};

static ETH_BRIDGE_INIT(bridge);

/**
 * @brief Setup the interface's IPv4 parameters.
 *
 * @param iface Pointer to network interface
 * @param iface_idx Interface index
 *
 * @return Error code
 */
static int setup_iface(struct net_if *iface, int iface_idx)
{
	const char *addr = ifs[iface_idx].addr;
	const char *gw = ifs[iface_idx].gw;
	const char *netmask = ifs[iface_idx].netmask;
	uint16_t vlan = ifs[iface_idx].vlan;
	struct in_addr inaddr;

	if (net_addr_pton(AF_INET, addr, &inaddr)) {
		printf("Error: Invalid address: %s\n", addr);
		return 1;
	}
	if (!net_if_ipv4_addr_add(iface, &inaddr, NET_ADDR_MANUAL, 0)) {
		printf("Error: Cannot add %s to interface\n", addr);
		return 1;
	}

	if (net_addr_pton(AF_INET, gw, &inaddr)) {
		printf("Error: Invalid gateway: %s\n", gw);
		return 1;
	}
	net_if_ipv4_set_gw(iface, &inaddr);

	if (net_addr_pton(AF_INET, netmask, &inaddr)) {
		printf("Error: Invalid netmask: %s\n", netmask);
		return 1;
	}
	net_if_ipv4_set_netmask(iface, &inaddr);

	if (vlan > 0) {
		int ret = net_eth_vlan_enable(iface, vlan);

		if (ret < 0) {
			printf("Error: Cannot set VLAN tag %u: %d\n", vlan, ret);
			return 1;
		}
	}

	return 0;
}

/**
 * @brief Setup the bridge for an interface.
 *
 * @param iface Pointer to network interface
 *
 * @return Error code
 */
static int setup_bridge(struct net_if *iface)
{
	int ret;

	ret = eth_bridge_iface_add(&bridge, iface);
	if (ret) {
		printf("Error: Cannot add iface %p to the bridge: %d\n", iface, ret);
		return 1;
	}

	ret = eth_bridge_iface_allow_tx(iface, true);
	if (ret) {
		printf("Error: Cannot enable tx for iface %p: %d\n", iface, ret);
		return 1;
	}

	return 0;
}

/**
 * @brief Configure the network interface.
 *
 * @param iface Pointer to network interface
 * @param iface_idx_ptr Pointer to interface index
 */
static void net_if_cb(struct net_if *iface, void *iface_idx_ptr)
{
	int iface_idx = (*(int *)iface_idx_ptr)++;
	bool iface_bridged = false;
	bool iface_setup = false;
	int ret;

	if (iface_idx >= ARRAY_SIZE(ifs)) {
		printf("Error: No configuration for interface %d\n", iface_idx + 1);
		return;
	}

	printf("  %d: %p\n", iface_idx + 1, iface);
	if (iface && iface->if_dev && iface->if_dev->dev && iface->if_dev->dev->name)
		printf("     Name:   %s\n", iface->if_dev->dev->name);

	if (ifs[iface_idx].addr && ifs[iface_idx].addr[0] != '\0') {
		ret = setup_iface(iface, iface_idx);
		if (ret)
			printf("Failed setting up iface %p\n", iface);
		else
			iface_setup = true;
	}

	if (ifs[iface_idx].bridge) {
		ret = setup_bridge(iface);
		if (!ret)
			iface_bridged = true;
	}

	if (iface_bridged) {
		printf("     Status: *Bridged*\n");
	} else if (iface_setup) {
		printf("     Status: Address: %s\n", ifs[iface_idx].addr);
		printf("             Netmask: %s\n", ifs[iface_idx].netmask);
		printf("             Gateway: %s\n", ifs[iface_idx].gw);
		printf("             VLAN   : %u\n", ifs[iface_idx].vlan);
	}
	printf("\n");
}

int main(void)
{
	int iface_idx = 0;

	printf("Network interfaces:\n");
	net_if_foreach(net_if_cb, &iface_idx);
	printf("Bridge initialization complete\n");

	return 0;
}

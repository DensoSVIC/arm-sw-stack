/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/shell/shell.h>
#include <zephyr/shell/shell_string_conv.h>

#include "zephyr/drivers/fault_mgmt/fault_mgmt_arm_fmu.h"
#include "zephyr/subsys/fault_mgmt/fault_mgmt.h"

static int cmd_fault_tree(const struct shell *sh, size_t argc, char **argv, void *data)
{
	for (int i = 0; i < ARRAY_SIZE(fault_mgmt_root_fmus); i++) {
		shell_print(sh, "Root: %s", fault_mgmt_root_fmus[i]->name);
	}

	return 0;
}

static int parse_fmu_args(const struct shell *sh, size_t argc, char **argv,
			  const struct device **dev, uint32_t *prot_id)
{
	int ret = 0;

	*dev = device_get_binding(argv[1]);
	if (*dev == NULL) {
		shell_error(sh, "Invalid device name: %s", argv[1]);
		return -EINVAL;
	}

	*prot_id = (uint32_t)shell_strtoul(argv[2], 0, &ret);
	if (ret < 0) {
		shell_error(sh, "Invalid protection ID: %s", argv[2]);
		return -EINVAL;
	}

	return 0;
}

static int handle_error(const struct shell *sh, int ec)
{
	switch (ec) {
	case 0:
		break;
	case -EINVAL:
		shell_error(sh, "Invalid argument");
		break;
	case -ENOTSUP:
		shell_error(sh, "Operation not supported");
		break;
	default:
		shell_error(sh, "Unknown error");
		break;
	}
	return ec;
}

static int cmd_fmu_inject(const struct shell *sh, size_t argc, char **argv, void *data)
{
	int ret;
	const struct device *dev;
	const struct fault_mgmt_arm_fmu_api *api;
	uint32_t prot_id;

	ret = parse_fmu_args(sh, argc, argv, &dev, &prot_id);
	if (ret < 0) {
		return ret;
	}

	shell_info(sh, "Injecting fault 0x%x to device %s", prot_id, dev->name);

	api = dev->api;
	ret = api->inject(dev, prot_id);
	return handle_error(sh, ret);
}

static int cmd_fmu_set_enabled(const struct shell *sh, size_t argc, char **argv, void *data)
{
	int ret;
	const struct device *dev;
	const struct fault_mgmt_arm_fmu_api *api;
	uint32_t prot_id;
	bool enabled;
	const char *action;

	ret = parse_fmu_args(sh, argc, argv, &dev, &prot_id);
	if (ret < 0) {
		return ret;
	}

	enabled = shell_strtobool(argv[3], 0, &ret);
	if (ret < 0) {
		shell_error(sh, "Invalid enabled status: %s", argv[3]);
		return -EINVAL;
	}

	action = enabled ? "Enabling" : "Disabling";
	shell_info(sh, "%s fault %x on device %s", action, prot_id, dev->name);

	api = dev->api;
	ret = api->set_enabled(dev, prot_id, enabled);
	return handle_error(sh, ret);
}

static void cmd_fmu_device_name(size_t idx, struct shell_static_entry *entry)
{
	const struct device *dev = shell_device_lookup(idx, NULL);

	entry->syntax = (dev != NULL) ? dev->name : NULL;
	entry->handler = NULL;
	entry->help = NULL;
	entry->subcmd = NULL;
}

SHELL_DYNAMIC_CMD_CREATE(dsub_device_name, cmd_fmu_device_name);

SHELL_STATIC_SUBCMD_SET_CREATE(
	fault, SHELL_CMD_ARG(tree, NULL, "Enumerate the fault tree", cmd_fault_tree, 1, 0),
	SHELL_CMD_ARG(inject, &dsub_device_name, "Inject a fault", cmd_fmu_inject, 3, 0),
	SHELL_CMD_ARG(set_enabled, &dsub_device_name, "Enable/disable a fault", cmd_fmu_set_enabled,
		      4, 0),
	SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(fault, &fault, "Fault management subsystem commands", NULL);

/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/shell/shell.h>
#include <zephyr/shell/shell_string_conv.h>

#include "zephyr/subsys/fault_mgmt/fault_mgmt.h"

#ifdef CONFIG_FAULT_MGMT_STORAGE_SYS_HASH_MAP
#include "zephyr/subsys/fault_mgmt/fault_mgmt_storage.h"
#endif

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
	uint32_t prot_id;

	ret = parse_fmu_args(sh, argc, argv, &dev, &prot_id);
	if (ret < 0) {
		return ret;
	}

	shell_info(sh, "Injecting fault 0x%x to device %s", prot_id, dev->name);

	ret = fault_mgmt_inject(dev, prot_id);
	return handle_error(sh, ret);
}

static int cmd_fmu_set_enabled(const struct shell *sh, size_t argc, char **argv, void *data)
{
	int ret;
	const struct device *dev;
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

	ret = fault_mgmt_set_enabled(dev, prot_id, enabled);
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

#ifdef CONFIG_FAULT_MGMT_STORAGE_SYS_HASH_MAP
void fault_history_read_callback(const struct fault_mgmt_storage_info *fault_info, void *cookie)
{
	const struct shell *sh = (const struct shell *)cookie;
	uint32_t protection_id = FAULT_MGMT_ARM_FMU_FAULT_PROTECTION_ID(&(fault_info->fault));
	const char *criticality = FAULT_MGMT_ARM_FMU_FAULT_IS_CRITICAL(&(fault_info->fault))
					  ? "critical"
					  : "non-critical";
	const struct device *dev = device_from_handle(fault_info->fault.handle);

	shell_info(sh, "Fault received (%s): 0x%x on %s : count %u\n", criticality, protection_id,
		   dev->name, fault_info->count);
}

static void cmd_fmu_fault_listed(const struct shell *sh)
{
	uint64_t record_size = fault_mgmt_storage_total_fault_reported();

	if (record_size == 0) {
		shell_info(sh, "No fault reported");
		return;
	}
	shell_info(sh, "Fault history:");
	fault_mgmt_storage_foreach(fault_history_read_callback, (void *)sh,
				   FAULT_MGMT_OPTION_LIST_FAULT);
}

static void cmd_fmu_fault_summary(const struct shell *sh)
{
	uint64_t record_size = fault_mgmt_storage_total_fault_reported();

	if (record_size == 0) {
		shell_info(sh, "No fault reported");
		return;
	}

	shell_info(sh,
		   "____________________________________________________________________________");
	shell_info(sh,
		   "|                                                                          |");
	shell_info(sh,
		   "|                                Fault Summary                             |");
	shell_info(sh,
		   "|__________________________________________________________________________|");
	shell_info(sh,
		   "                                                                            ");
	shell_info(sh, "Number of fault reported: %llu", record_size);

	shell_info(sh,
		   "____________________________________________________________________________");
	shell_info(sh,
		   "                                                                            ");
	shell_info(sh, "Most reported faults:");
	fault_mgmt_storage_foreach(fault_history_read_callback, (void *)sh,
				   FAULT_MGMT_OPTION_LIST_MOST_REPORTED_FAULT);

	shell_info(sh,
		   "____________________________________________________________________________");
	shell_info(sh,
		   "                                                                            ");
	shell_info(sh, "Fault history:");
	fault_mgmt_storage_foreach(fault_history_read_callback, (void *)sh,
				   FAULT_MGMT_OPTION_LIST_FAULT);
	shell_info(sh,
		   "____________________________________________________________________________");
}

static void cmd_fmu_total_reported_fault(const struct shell *sh)
{
	uint64_t record_size = fault_mgmt_storage_total_fault_reported();

	shell_info(sh, "Number of fault reported: %llu", record_size);
}

static void cmd_fmu_clear_stored_fault(const struct shell *sh)
{
	shell_info(sh, "Erasing the storage...");
	fault_mgmt_storage_clear();
	shell_info(sh, "Done!");
}
#endif

SHELL_DYNAMIC_CMD_CREATE(dsub_device_name, cmd_fmu_device_name);

SHELL_STATIC_SUBCMD_SET_CREATE(
	fault, SHELL_CMD_ARG(tree, NULL, "Enumerate the fault tree", cmd_fault_tree, 1, 0),
	SHELL_CMD_ARG(inject, &dsub_device_name, "Inject a fault", cmd_fmu_inject, 3, 0),
	SHELL_CMD_ARG(set_enabled, &dsub_device_name, "Enable/disable a fault", cmd_fmu_set_enabled,
		      4, 0),
#ifdef CONFIG_FAULT_MGMT_STORAGE_SYS_HASH_MAP
	SHELL_CMD_ARG(list, NULL, "List all reported faults", cmd_fmu_fault_listed, 0, 0),
	SHELL_CMD_ARG(summary, NULL, "Show fault summary", cmd_fmu_fault_summary, 0, 0),
	SHELL_CMD_ARG(count, NULL, "Total faults reported", cmd_fmu_total_reported_fault, 0, 0),
	SHELL_CMD_ARG(clear, NULL, "Clear the storage", cmd_fmu_clear_stored_fault, 0, 0),
#endif
	SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(fault, &fault, "Fault management subsystem commands", NULL);

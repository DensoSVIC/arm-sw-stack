/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(fault_mgmt_arm_ssu, CONFIG_FAULT_MGMT_LOG_LEVEL);

#include <zephyr/devicetree.h>
#include <zephyr/kernel.h>

#include "zephyr/drivers/fault_mgmt/fault_mgmt_safety_device.h"

#define FAULT_MGMT_ARM_SSU_FIELD_STATUS 0x20
#define FAULT_MGMT_ARM_SSU_FIELD_CNTRL  0x28
#define FAULT_MGMT_ARM_SSU_STATUS_MASK  GENMASK(2, 0)
#define FAULT_MGMT_ARM_SSU_CNTRL_MASK   GENMASK(1, 0)

const char *fault_mgmt_safety_state_to_str(enum fault_mgmt_safety_state val)
{
	switch (val) {
	case FAULT_MGMT_SAFETY_STATUS_TEST:
		return "TEST";
	case FAULT_MGMT_SAFETY_STATUS_SAFE:
		return "SAFE";
	case FAULT_MGMT_SAFETY_STATUS_ERRN:
		return "ERRN";
	case FAULT_MGMT_SAFETY_STATUS_ERRC:
		return "ERRC";
	default:
		return NULL;
	}
}

int fault_mgmt_safety_signal_from_str(const char *str, enum fault_mgmt_safety_signal *val)
{
	if (strcmp(str, "compl_ok") == 0) {
		*val = FAULT_MGMT_SAFETY_CNTRL_COMPL_OK;
	} else if (strcmp(str, "nce_ok") == 0) {
		*val = FAULT_MGMT_SAFETY_CNTRL_NCE_OK;
	} else if (strcmp(str, "ce_not_ok") == 0) {
		*val = FAULT_MGMT_SAFETY_CNTRL_CE_NOT_OK;
	} else if (strcmp(str, "nce_not_ok") == 0) {
		*val = FAULT_MGMT_SAFETY_CNTRL_NCE_NOT_OK;
	} else {
		return -EINVAL;
	}

	return 0;
}

struct fault_mgmt_arm_ssu_config {
	DEVICE_MMIO_ROM;
};

struct fault_mgmt_arm_ssu_data {
	DEVICE_MMIO_RAM;
};

static int fault_mgmt_arm_ssu_init(const struct device *dev)
{
	DEVICE_MMIO_MAP(dev, K_MEM_CACHE_NONE);

	return 0;
}

static enum fault_mgmt_safety_state fault_mgmt_arm_ssu_status(const struct device *dev)
{
	return sys_read32(DEVICE_MMIO_GET(dev) + FAULT_MGMT_ARM_SSU_FIELD_STATUS) &
	       FAULT_MGMT_ARM_SSU_STATUS_MASK;
}

static void fault_mgmt_arm_ssu_control(const struct device *dev, enum fault_mgmt_safety_signal val)
{
	sys_write32(val & FAULT_MGMT_ARM_SSU_CNTRL_MASK,
		    DEVICE_MMIO_GET(dev) + FAULT_MGMT_ARM_SSU_FIELD_CNTRL);
}

static const struct fault_mgmt_safety_device_api fault_mgmt_arm_ssu_api = {
	.status = fault_mgmt_arm_ssu_status,
	.control = fault_mgmt_arm_ssu_control,
};

#define DT_DRV_COMPAT arm_ssu

#define FAULT_MGMT_ARM_SSU_INIT(n)                                                                 \
	static const struct fault_mgmt_arm_ssu_config fault_mgmt_arm_ssu_config_##n = {            \
		DEVICE_MMIO_ROM_INIT(DT_DRV_INST(n)),                                              \
	};                                                                                         \
	static struct fault_mgmt_arm_ssu_data fault_mgmt_arm_ssu_data_##n = {};                    \
	DEVICE_DT_INST_DEFINE(n, &fault_mgmt_arm_ssu_init, NULL, &fault_mgmt_arm_ssu_data_##n,     \
			      &fault_mgmt_arm_ssu_config_##n, POST_KERNEL,                         \
			      CONFIG_KERNEL_INIT_PRIORITY_DEVICE, &fault_mgmt_arm_ssu_api);

DT_INST_FOREACH_STATUS_OKAY(FAULT_MGMT_ARM_SSU_INIT);

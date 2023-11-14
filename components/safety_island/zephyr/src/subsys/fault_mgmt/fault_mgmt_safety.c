/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(fault_mgmt_safety, CONFIG_FAULT_MGMT_LOG_LEVEL);

#include "zephyr/subsys/fault_mgmt/fault_mgmt.h"
#include "zephyr/subsys/fault_mgmt/fault_mgmt_safety.h"

enum fault_mgmt_safety_state fault_mgmt_safety_status(const struct device *dev)
{
	return FAULT_MGMT_SAFETY_DEV_API(dev)->status(dev);
}

void fault_mgmt_safety_control(const struct device *dev, enum fault_mgmt_safety_signal val)
{
	FAULT_MGMT_SAFETY_DEV_API(dev)->control(dev, val);
}

static int fault_mgmt_safety_init(const struct device *root_dev)
{
	size_t count = 0;
	const struct device *safety_dev;
	safety_dev = fault_mgmt_safety_device(root_dev);

	/* Ensure the root device has an attached safety device which is ready */
	if (!safety_dev || !device_is_ready(safety_dev)) {
		LOG_ERR("Safety device %s is not ready\n", safety_dev->name);
		return -ENODEV;
	}

	device_supported_handles_get(safety_dev, &count);
	if (count > 1) {
		LOG_ERR("Safety device %s referenced by multiple devices\n", safety_dev->name);
		return -EINVAL;
	}

	LOG_DBG("Fault management safety handler initialized for: %s\n", safety_dev->name);

	return 0;
}

static void fault_mgmt_safety_handle(const struct device *root_dev,
				     const struct fault_mgmt_fault *fault)
{
	const struct device *safety_dev;
	enum fault_mgmt_safety_state state;

	safety_dev = fault_mgmt_safety_device(root_dev);
	state = fault_mgmt_safety_status(safety_dev);
	LOG_INF("Safety status: %s (0x%x) on %s\n", fault_mgmt_safety_state_to_str(state), state,
		safety_dev->name);
}

FAULT_MGMT_HANDLER_DEFINE(CONFIG_FAULT_MGMT_SAFETY_PRIORITY, fault_mgmt_safety_init,
			  fault_mgmt_safety_handle);

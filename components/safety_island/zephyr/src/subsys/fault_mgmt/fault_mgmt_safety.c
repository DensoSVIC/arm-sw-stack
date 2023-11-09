/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(fault_mgmt_safety, CONFIG_FAULT_MGMT_LOG_LEVEL);

#include "zephyr/subsys/fault_mgmt/fault_mgmt_safety.h"

enum fault_mgmt_safety_state fault_mgmt_safety_status(const struct device *dev)
{
	return FAULT_MGMT_SAFETY_DEV_API(dev)->status(dev);
}

void fault_mgmt_safety_control(const struct device *dev, enum fault_mgmt_safety_signal val)
{
	FAULT_MGMT_SAFETY_DEV_API(dev)->control(dev, val);
}

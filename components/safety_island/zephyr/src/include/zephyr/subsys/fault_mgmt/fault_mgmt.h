/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FAULT_MGMT_H_
#define FAULT_MGMT_H_

#include <zephyr/device.h>

typedef int (*fault_mgmt_device_callback)(const struct device *dev, size_t depth, size_t index,
					  void *cookie);
int fault_mgmt_device_foreach(fault_mgmt_device_callback callback, void *cookie);

int fault_mgmt_inject(const struct device *dev, uint32_t prot_id);
int fault_mgmt_set_enabled(const struct device *dev, uint32_t prot_id, bool enabled);

#endif /* FAULT_MGMT_H_ */

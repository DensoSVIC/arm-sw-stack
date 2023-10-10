/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FAULT_MGMT_H_
#define FAULT_MGMT_H_

#include <zephyr/devicetree.h>

#define ZEPHYR_USER_NODE DT_PATH(zephyr_user)

extern const struct device *fault_mgmt_root_fmus[DT_PROP_LEN(ZEPHYR_USER_NODE, root_fmus)];

int fault_mgmt_inject(const struct device *dev, uint32_t prot_id);
int fault_mgmt_set_enabled(const struct device *dev, uint32_t prot_id, bool enabled);

#endif /* FAULT_MGMT_H_ */

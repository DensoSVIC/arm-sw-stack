/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_FAULT_MGMT_ARM_FMU_H_
#define ZEPHYR_INCLUDE_FAULT_MGMT_ARM_FMU_H_

#include <zephyr/device.h>

struct fault_mgmt_arm_fmu_fault {
	device_handle_t handle;
	uint32_t prot_id;
};

#define FAULT_MGMT_ARM_FMU_FAULT_CRITICAL_MASK      BIT(31)
#define FAULT_MGMT_ARM_FMU_FAULT_PROTECTION_ID_MASK GENMASK(30, 0)
#define FAULT_MGMT_ARM_FMU_FAULT_IS_CRITICAL(fault)                                                \
	(((fault)->prot_id & FAULT_MGMT_ARM_FMU_FAULT_CRITICAL_MASK) > 0)
#define FAULT_MGMT_ARM_FMU_FAULT_PROTECTION_ID(fault)                                              \
	((fault)->prot_id & FAULT_MGMT_ARM_FMU_FAULT_PROTECTION_ID_MASK)
#define FAULT_MGMT_ARM_FMU_FAULT_PROTECTION_ID_INVALID 0

typedef void (*fault_mgmt_arm_fmu_callback_t)(const struct device *dev,
					      const struct fault_mgmt_arm_fmu_fault *fault,
					      void *user_data);

__subsystem struct fault_mgmt_arm_fmu_api {
	int (*inject)(const struct device *dev, uint32_t prot_id);
	int (*set_enabled)(const struct device *dev, uint32_t prot_id, bool enabled);
	int (*fault_callback_set)(const struct device *dev, fault_mgmt_arm_fmu_callback_t callback,
				  void *user_data);
};

#define FAULT_MGMT_ARM_FMU_DEV_API(dev) ((const struct fault_mgmt_arm_fmu_api *const)(dev)->api)

#endif /* ZEPHYR_INCLUDE_FAULT_MGMT_ARM_FMU_H_ */

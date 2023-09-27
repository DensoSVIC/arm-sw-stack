/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_FAULT_MGMT_ARM_FMU_PRIV_H_
#define ZEPHYR_DRIVERS_FAULT_MGMT_ARM_FMU_PRIV_H_

#include <zephyr/sys/iterable_sections.h>

struct fault_mgmt_arm_fmu_internal_api {
	void (*isr)(const struct device *dev, bool critical);
	int (*inject)(const struct device *dev, uint32_t prot_id);
	int (*set_enabled)(const struct device *dev, uint32_t prot_id, bool enabled);
};

struct fault_mgmt_arm_fmu_implementation {
	uint32_t erriidr;
	int (*init_fn)(const struct device *dev);
	const struct fault_mgmt_arm_fmu_internal_api *api;
};

#define FAULT_MGMT_ARM_FMU_DEFINE(name, _erriidr, _init_fn, _api)                                  \
	STRUCT_SECTION_ITERABLE(fault_mgmt_arm_fmu_implementation, name) = {                       \
		.erriidr = _erriidr,                                                               \
		.init_fn = _init_fn,                                                               \
		.api = _api,                                                                       \
	}

struct fault_mgmt_arm_fmu_config {
	DEVICE_MMIO_ROM;
	void (*irq_config)(const struct device *dev);
};

struct fault_mgmt_arm_fmu_data {
	DEVICE_MMIO_RAM;
	struct k_spinlock lock;
	fault_mgmt_arm_fmu_callback_t callback;
	void *user_data;
	const struct fault_mgmt_arm_fmu_internal_api *internal_api;
};

#define FAULT_MGMT_ARM_FMU_DEV_DATA(dev) ((struct fault_mgmt_arm_fmu_data *const)(dev)->data)
#define FAULT_MGMT_ARM_FMU_DEV_CFG(dev)                                                            \
	((const struct fault_mgmt_arm_fmu_config *const)(dev)->config)

#define FAULT_MGMT_ARM_FMU_FIELD_ERRGSR  0xE00
#define FAULT_MGMT_ARM_FMU_FIELD_ERRGSR2 0xE04
#define FAULT_MGMT_ARM_FMU_FIELD_ERRIIDR 0xE10
#define FAULT_MGMT_ARM_FMU_FIELD_SMEN    0xF00
#define FAULT_MGMT_ARM_FMU_FIELD_SMERR   0xF04
#define FAULT_MGMT_ARM_FMU_FIELD_FMU_KEY 0xF20
static const mem_addr_t FAULT_MGMT_ARM_FMU_FIELD_PID[] = {0xFE0, 0xFE4, 0xFE8, 0xFEC};
static const mem_addr_t FAULT_MGMT_ARM_FMU_FIELD_CID[] = {0xFF0, 0xFF4, 0xFF8, 0xFFC};

#define FAULT_MGMT_ARM_FMU_RECORD_FIELD(record_id, offset) ((record_id)*0x40 + (offset))
#define FAULT_MGMT_ARM_FMU_RECORD_FIELD_FR(record_id)                                              \
	FAULT_MGMT_ARM_FMU_RECORD_FIELD(record_id, 0x0)
#define FAULT_MGMT_ARM_FMU_RECORD_FIELD_CONTROL(record_id)                                         \
	FAULT_MGMT_ARM_FMU_RECORD_FIELD(record_id, 0x8)
#define FAULT_MGMT_ARM_FMU_RECORD_FIELD_STATUS(record_id)                                          \
	FAULT_MGMT_ARM_FMU_RECORD_FIELD(record_id, 0x10)

#define FAULT_MGMT_ARM_FMU_FR_ED_MASK         GENMASK(1, 0)
#define FAULT_MGMT_ARM_FMU_FR_ED_INTERNAL     0x2
#define FAULT_MGMT_ARM_FMU_FR_ED_UPSTREAM     0x0
#define FAULT_MGMT_ARM_FMU_FR_CI_MASK         GENMASK(23, 22)
#define FAULT_MGMT_ARM_FMU_FR_CI_CRITICAL     0x3
#define FAULT_MGMT_ARM_FMU_FR_CI_NON_CRITICAL 0x0
#define FAULT_MGMT_ARM_FMU_STATUS_V_MASK      BIT(30)
#define FAULT_MGMT_ARM_FMU_STATUS_IERR_MASK   GENMASK(17, 8)
#define FAULT_MGMT_ARM_FMU_KEY                0xBE
#define FAULT_MGMT_ARM_FMU_CID_AMBA           0xB105F00D
#define FAULT_MGMT_ARM_FMU_PID_SYSTEM         0x0BB49B
#define FAULT_MGMT_ARM_FMU_PID_MASK           0xFFFFF

#define FAULT_MGMT_ARM_FMU_SYSTEM_ERRIIDR 0x10000

static ALWAYS_INLINE uint32_t fault_mgmt_arm_fmu_read32(const struct device *dev, mem_addr_t offset)
{
	return sys_read32(DEVICE_MMIO_GET(dev) + offset);
}

static ALWAYS_INLINE void fault_mgmt_arm_fmu_write32(const struct device *dev, uint32_t value,
						     mem_addr_t offset)
{
	/* A specific value must be written to the key register to allow a single
	 * subsequent write to any other register
	 */
	sys_write32(FAULT_MGMT_ARM_FMU_KEY,
		    DEVICE_MMIO_GET(dev) + FAULT_MGMT_ARM_FMU_FIELD_FMU_KEY);
	sys_write32(value, DEVICE_MMIO_GET(dev) + offset);
}

#endif /* ZEPHYR_DRIVERS_FAULT_MGMT_ARM_FMU_PRIV_H_ */

/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(fault_mgmt_arm_fmu, CONFIG_FAULT_MGMT_LOG_LEVEL);

#include <zephyr/devicetree.h>
#include <zephyr/kernel.h>

#include "zephyr/drivers/fault_mgmt/fault_mgmt_arm_fmu.h"
#include "fault_mgmt_arm_fmu_priv.h"

static int fault_mgmt_arm_system_fmu_init(const struct device *dev)
{
	/* Enable all internal faults initially */
	fault_mgmt_arm_fmu_write32(dev, BIT_MASK(32), FAULT_MGMT_ARM_FMU_FIELD_SMEN);

	return 0;
}

static void fault_mgmt_arm_fmu_handle_internal_error(const struct device *dev, uint32_t record_id)
{
	struct fault_mgmt_arm_fmu_data *data = FAULT_MGMT_ARM_FMU_DEV_DATA(dev);
	uint32_t status, ierr, smerr;
	struct fault_mgmt_arm_fmu_fault fault = {
		.handle = device_handle_get(dev),
	};

	status = fault_mgmt_arm_fmu_read32(dev, FAULT_MGMT_ARM_FMU_RECORD_FIELD_STATUS(record_id));
	if (FIELD_GET(FAULT_MGMT_ARM_FMU_STATUS_V_MASK, status) == 0) {
		LOG_WRN("Spurious interrupt\n");
		return;
	}

	smerr = fault_mgmt_arm_fmu_read32(dev, FAULT_MGMT_ARM_FMU_FIELD_SMERR);
	ierr = FIELD_GET(FAULT_MGMT_ARM_FMU_STATUS_IERR_MASK, status);
	while (ierr) {
		fault.prot_id = LSB_GET(ierr);
		LOG_DBG("Fault detected: 0x%x\n", fault.prot_id);
		ierr &= ~fault.prot_id;
		smerr &= ~fault.prot_id;

		if (data->callback) {
			data->callback(dev, &fault, data->user_data);
		}
	}

	status &= ~FAULT_MGMT_ARM_FMU_STATUS_IERR_MASK;
	fault_mgmt_arm_fmu_write32(dev, status, FAULT_MGMT_ARM_FMU_RECORD_FIELD_STATUS(record_id));
	fault_mgmt_arm_fmu_write32(dev, smerr, FAULT_MGMT_ARM_FMU_FIELD_SMERR);
}

static void fault_mgmt_arm_fmu_system_isr(const struct device *dev, bool critical)
{
	uint32_t record_id, features;
	uint64_t errgsrs, lsb;
	bool is_critical, is_internal;

	errgsrs = fault_mgmt_arm_fmu_read32(dev, FAULT_MGMT_ARM_FMU_FIELD_ERRGSR);
	errgsrs |= ((uint64_t)fault_mgmt_arm_fmu_read32(dev, FAULT_MGMT_ARM_FMU_FIELD_ERRGSR2))
		   << 32;

	while (errgsrs) {
		lsb = LSB_GET(errgsrs);
		record_id = LOG2(lsb);
		features = fault_mgmt_arm_fmu_read32(dev,
						     FAULT_MGMT_ARM_FMU_RECORD_FIELD_FR(record_id));
		is_critical = FIELD_GET(FAULT_MGMT_ARM_FMU_FR_CI_MASK, features) ==
			      FAULT_MGMT_ARM_FMU_FR_CI_CRITICAL;
		if (is_critical == critical) {
			is_internal = FIELD_GET(FAULT_MGMT_ARM_FMU_FR_ED_MASK, features) ==
				      FAULT_MGMT_ARM_FMU_FR_ED_INTERNAL;
			if (is_internal) {
				fault_mgmt_arm_fmu_handle_internal_error(dev, record_id);
			} else {
				LOG_WRN("Unsupported error record: 0x%x\n", record_id);
			}
		}
		errgsrs &= ~lsb;
	}
}

static int fault_mgmt_arm_fmu_system_validate_prot_id(uint32_t prot_id)
{
	if (!IS_POWER_OF_TWO(prot_id)) {
		LOG_DBG("Unsupported protection ID: 0x%x\n", prot_id);
		return -EINVAL;
	}

	return 0;
}

static int fault_mgmt_arm_fmu_system_inject(const struct device *dev, uint32_t prot_id)
{
	int ret;
	uint32_t smerr;

	ret = fault_mgmt_arm_fmu_system_validate_prot_id(prot_id);
	if (ret < 0) {
		return ret;
	}

	LOG_DBG("Injecting error 0x%x\n", prot_id);

	smerr = fault_mgmt_arm_fmu_read32(dev, FAULT_MGMT_ARM_FMU_FIELD_SMERR);
	fault_mgmt_arm_fmu_write32(dev, smerr | prot_id, FAULT_MGMT_ARM_FMU_FIELD_SMERR);

	return 0;
}

static int fault_mgmt_arm_fmu_system_set_enabled(const struct device *dev, uint32_t prot_id,
						 bool enabled)
{
	int ret;
	uint32_t smen;

	ret = fault_mgmt_arm_fmu_system_validate_prot_id(prot_id);
	if (ret < 0) {
		return ret;
	}

	LOG_DBG("Changing enabled status of 0x%x\n", prot_id);

	smen = fault_mgmt_arm_fmu_read32(dev, FAULT_MGMT_ARM_FMU_FIELD_SMEN);
	smen = enabled ? smen | prot_id : smen & ~prot_id;
	fault_mgmt_arm_fmu_write32(dev, smen, FAULT_MGMT_ARM_FMU_FIELD_SMEN);

	return 0;
}

static const struct fault_mgmt_arm_fmu_internal_api fault_mgmt_arm_fmu_system_api = {
	.isr = fault_mgmt_arm_fmu_system_isr,
	.inject = fault_mgmt_arm_fmu_system_inject,
	.set_enabled = fault_mgmt_arm_fmu_system_set_enabled,
};

FAULT_MGMT_ARM_FMU_DEFINE(system_fmu, FAULT_MGMT_ARM_FMU_SYSTEM_ERRIIDR,
			  &fault_mgmt_arm_system_fmu_init, &fault_mgmt_arm_fmu_system_api);

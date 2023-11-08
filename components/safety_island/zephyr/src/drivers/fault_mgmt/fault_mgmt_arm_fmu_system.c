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

static int fault_mgmt_arm_fmu_next_internal_fault(const struct device *dev, uint32_t record_id,
						  uint32_t *prot_id)
{
	uint32_t status, ierr, smerr;

	status = fault_mgmt_arm_fmu_read32(dev, FAULT_MGMT_ARM_FMU_RECORD_FIELD_STATUS(record_id));
	if (FIELD_GET(FAULT_MGMT_ARM_FMU_STATUS_V_MASK, status) == 0) {
		LOG_WRN("Spurious interrupt\n");
		return -EFAULT;
	}

	smerr = fault_mgmt_arm_fmu_read32(dev, FAULT_MGMT_ARM_FMU_FIELD_SMERR);
	ierr = FIELD_GET(FAULT_MGMT_ARM_FMU_STATUS_IERR_MASK, status);
	*prot_id = LSB_GET(ierr);
	LOG_DBG("Fault detected: 0x%x\n", *prot_id);
	ierr &= ~(*prot_id);
	smerr &= ~(*prot_id);

	status &= ~FAULT_MGMT_ARM_FMU_STATUS_IERR_MASK;
	fault_mgmt_arm_fmu_write32(dev, status, FAULT_MGMT_ARM_FMU_RECORD_FIELD_STATUS(record_id));
	fault_mgmt_arm_fmu_write32(dev, smerr, FAULT_MGMT_ARM_FMU_FIELD_SMERR);

	return 0;
}

static int fault_mgmt_arm_fmu_next_upstream_fault(const struct device *dev, uint32_t record_id,
						  uint32_t *next_id)
{
	size_t count = 0;
	const device_handle_t *upstream_fmus;
	uint32_t upstream_fmu_index = record_id / 2;
	uint32_t status;

	upstream_fmus = device_required_handles_get(dev, &count);
	if (upstream_fmu_index >= count) {
		LOG_ERR("Upstream FMU %d does not exist\n", upstream_fmu_index);
		k_oops();
	}

	status = fault_mgmt_arm_fmu_read32(dev, FAULT_MGMT_ARM_FMU_RECORD_FIELD_STATUS(record_id));
	if (FIELD_GET(FAULT_MGMT_ARM_FMU_STATUS_V_MASK, status) == 0) {
		LOG_WRN("Spurious interrupt\n");
		return -EFAULT;
	}

	LOG_DBG("Upstream fault: %d\n", upstream_fmu_index);
	*next_id = upstream_fmus[upstream_fmu_index];

	/* Set the V bit to clear the fault */
	fault_mgmt_arm_fmu_write32(dev, status, FAULT_MGMT_ARM_FMU_RECORD_FIELD_STATUS(record_id));

	return FAULT_MGMT_ARM_FMU_NEXT_FAULT_UPSTREAM;
}

static int fault_mgmt_arm_fmu_system_next_fault(const struct device *dev, bool critical,
						uint32_t *next_id)
{
	uint32_t record_id, features;
	uint64_t errgsrs, lsb;
	bool is_internal;

	errgsrs = fault_mgmt_arm_fmu_read32(dev, FAULT_MGMT_ARM_FMU_FIELD_ERRGSR);
	errgsrs |= ((uint64_t)fault_mgmt_arm_fmu_read32(dev, FAULT_MGMT_ARM_FMU_FIELD_ERRGSR2))
		   << 32;
	errgsrs &= critical ? FAULT_MGMT_ARM_FMU_ERRGSR_CRITICAL_MASK
			    : FAULT_MGMT_ARM_FMU_ERRGSR_NON_CRITICAL_MASK;
	if (errgsrs == 0) {
		return 0;
	}

	lsb = LSB_GET(errgsrs);
	record_id = LOG2(lsb);
	features = fault_mgmt_arm_fmu_read32(dev, FAULT_MGMT_ARM_FMU_RECORD_FIELD_FR(record_id));
	is_internal = FIELD_GET(FAULT_MGMT_ARM_FMU_FR_ED_MASK, features) ==
		      FAULT_MGMT_ARM_FMU_FR_ED_INTERNAL;
	if (is_internal) {
		return fault_mgmt_arm_fmu_next_internal_fault(dev, record_id, next_id);
	} else {
		return fault_mgmt_arm_fmu_next_upstream_fault(dev, record_id, next_id);
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

static int fault_mgmt_arm_fmu_system_set_critical(const struct device *dev, uint32_t prot_id,
						  bool critical)
{
	/* All System FMU internal faults are always non-critical */
	return critical ? -ENOTSUP : 0;
}

static const struct fault_mgmt_arm_fmu_internal_api fault_mgmt_arm_fmu_system_api = {
	.next_fault = fault_mgmt_arm_fmu_system_next_fault,
	.inject = fault_mgmt_arm_fmu_system_inject,
	.set_enabled = fault_mgmt_arm_fmu_system_set_enabled,
	.set_critical = fault_mgmt_arm_fmu_system_set_critical,
};

FAULT_MGMT_ARM_FMU_DEFINE(system_fmu, FAULT_MGMT_ARM_FMU_SYSTEM_ERRIIDR,
			  &fault_mgmt_arm_system_fmu_init, &fault_mgmt_arm_fmu_system_api);

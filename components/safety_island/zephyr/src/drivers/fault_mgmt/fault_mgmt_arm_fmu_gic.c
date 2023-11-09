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

static int fault_mgmt_arm_fmu_device_wait_busy(const struct device *dev)
{
	uint32_t status;

	/* Poll while busy */
	WAIT_FOR((fault_mgmt_arm_fmu_read32(dev, FAULT_MGMT_ARM_FMU_FIELD_FMU_STATUS) &
		  FAULT_MGMT_ARM_FMU_STATUS_BUSY_MASK) == 0,
		 100, k_busy_wait(1));

	status = fault_mgmt_arm_fmu_read32(dev, FAULT_MGMT_ARM_FMU_FIELD_FMU_STATUS);

	/* Inspect the resultant status */
	if (status & FAULT_MGMT_ARM_FMU_STATUS_TIMEOUT_MASK) {
		LOG_ERR("FMU transaction timed out\n");
		return -ETIMEDOUT;
	} else if (status & FAULT_MGMT_ARM_FMU_STATUS_BLKID_PWROFF_MASK) {
		LOG_ERR("FMU block powered off\n");
		return -EIO;
	} else if (status & FAULT_MGMT_ARM_FMU_STATUS_BLKID_ERR_MASK) {
		LOG_ERR("Invalid FMU block ID\n");
		return -EINVAL;
	} else if (status & FAULT_MGMT_ARM_FMU_STATUS_PROTID_ERR_MASK) {
		LOG_ERR("Invalid FMU protection ID\n");
		return -EINVAL;
	}

	return 0;
}

static int fault_mgmt_arm_fmu_device_update(const struct device *dev, uint32_t record_id,
					    bool critical)
{
	uint32_t errupdate = FIELD_PREP(FAULT_MGMT_ARM_FMU_ERRUPDATE_RECORD_PAIR_MASK, record_id) |
			     (critical ? FAULT_MGMT_ARM_FMU_ERRUPDATE_CRITICAL_MASK
				       : FAULT_MGMT_ARM_FMU_ERRUPDATE_NON_CRITICAL_MASK);

	fault_mgmt_arm_fmu_write32(dev, errupdate, FAULT_MGMT_ARM_FMU_FIELD_ERRUPDATE);
	return fault_mgmt_arm_fmu_device_wait_busy(dev);
}

static int fault_mgmt_arm_fmu_device_next_fault(const struct device *dev, bool critical,
						uint32_t *next_id)
{
	uint32_t record_id, blktype, blkid, smid;
	uint64_t errgsr, lsb, status;

	errgsr = fault_mgmt_arm_fmu_read64(dev, FAULT_MGMT_ARM_FMU_FIELD_ERRGSR);
	errgsr &= critical ? FAULT_MGMT_ARM_FMU_ERRGSR_CRITICAL_MASK
			   : FAULT_MGMT_ARM_FMU_ERRGSR_NON_CRITICAL_MASK;
	if (errgsr == 0) {
		return 0;
	}

	lsb = LSB_GET(errgsr);
	record_id = LOG2(lsb);
	status = fault_mgmt_arm_fmu_read64(dev, FAULT_MGMT_ARM_FMU_RECORD_FIELD_STATUS(record_id));
	if (FIELD_GET(FAULT_MGMT_ARM_FMU_STATUS_V_MASK, status) == 0) {
		return 0;
	}

	blktype = record_id / 2;
	blkid = FIELD_GET(FAULT_MGMT_ARM_FMU_STATUS_BLKID_MASK, status);
	smid = FIELD_GET(FAULT_MGMT_ARM_FMU_STATUS_IERR_MASK, status);
	*next_id = FIELD_PREP(FAULT_MGMT_ARM_FMU_GIC_BLKTYPE_MASK, blktype) |
		   FIELD_PREP(FAULT_MGMT_ARM_FMU_GIC_BLKID_MASK, blkid) |
		   FIELD_PREP(FAULT_MGMT_ARM_FMU_GIC_SMID_MASK, smid) |
		   (critical ? FAULT_MGMT_ARM_FMU_FAULT_CRITICAL_MASK : 0x0);
	LOG_DBG("Fault detected: 0x%x\n", *next_id);

	status &= ~FAULT_MGMT_ARM_FMU_STATUS_V_MASK;
	fault_mgmt_arm_fmu_write64(dev, status, FAULT_MGMT_ARM_FMU_RECORD_FIELD_STATUS(record_id));
	fault_mgmt_arm_fmu_device_wait_busy(dev);

	/* Check if error buffer has overflowed and should be updated */
	if (status & FAULT_MGMT_ARM_FMU_STATUS_OFX_MASK) {
		fault_mgmt_arm_fmu_device_update(dev, record_id, critical);
	}

	return 0;
}

static int fault_mgmt_arm_fmu_device_validate_prot_id(uint32_t prot_id)
{
	if (prot_id & ~(FAULT_MGMT_ARM_FMU_GIC_BLKTYPE_MASK | FAULT_MGMT_ARM_FMU_GIC_BLKID_MASK |
			FAULT_MGMT_ARM_FMU_GIC_SMID_MASK)) {
		LOG_DBG("Unsupported protection ID: 0x%x\n", prot_id);
		return -EINVAL;
	}

	return 0;
}

static int fault_mgmt_arm_fmu_device_inject(const struct device *dev, uint32_t prot_id)
{
	int ret;

	ret = fault_mgmt_arm_fmu_device_validate_prot_id(prot_id);
	if (ret < 0) {
		return ret;
	}

	LOG_DBG("Injecting error 0x%x\n", prot_id);

	fault_mgmt_arm_fmu_write32(dev, prot_id, FAULT_MGMT_ARM_FMU_FIELD_SMERR);
	return fault_mgmt_arm_fmu_device_wait_busy(dev);
}

static int fault_mgmt_arm_fmu_device_set_enabled(const struct device *dev, uint32_t prot_id,
						 bool enabled)
{
	int ret;
	uint32_t smen;

	ret = fault_mgmt_arm_fmu_device_validate_prot_id(prot_id);
	if (ret < 0) {
		return ret;
	}

	LOG_DBG("Changing enabled status of 0x%x\n", prot_id);

	smen = enabled ? prot_id | FAULT_MGMT_ARM_FMU_GIC_ENABLED
		       : prot_id & ~FAULT_MGMT_ARM_FMU_GIC_ENABLED;
	fault_mgmt_arm_fmu_write32(dev, smen, FAULT_MGMT_ARM_FMU_FIELD_SMEN);
	return fault_mgmt_arm_fmu_device_wait_busy(dev);
}

static const struct fault_mgmt_arm_fmu_internal_api fault_mgmt_arm_fmu_device_api = {
	.next_fault = fault_mgmt_arm_fmu_device_next_fault,
	.inject = fault_mgmt_arm_fmu_device_inject,
	.set_enabled = fault_mgmt_arm_fmu_device_set_enabled,
};

FAULT_MGMT_ARM_FMU_DEFINE(gic_fmu, FAULT_MGMT_ARM_FMU_GIC_ERRIIDR, NULL,
			  &fault_mgmt_arm_fmu_device_api);

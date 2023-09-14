/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "zephyr/drivers/fault_mgmt/fault_mgmt_arm_fmu.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(fault_mgmt_arm_fmu, CONFIG_FAULT_MGMT_LOG_LEVEL);

#include <zephyr/devicetree.h>
#include <zephyr/kernel.h>
#include <zephyr/spinlock.h>

#include "fault_mgmt_arm_fmu_priv.h"

static int fault_mgmt_arm_fmu_init(const struct device *dev)
{
	const struct fault_mgmt_arm_fmu_config *cfg = FAULT_MGMT_ARM_FMU_DEV_CFG(dev);
	struct fault_mgmt_arm_fmu_data *data = FAULT_MGMT_ARM_FMU_DEV_DATA(dev);
	k_spinlock_key_t key = k_spin_lock(&data->lock);

	DEVICE_MMIO_MAP(dev, K_MEM_CACHE_NONE);

	if (cfg->irq_config) {
		cfg->irq_config(dev);
	}

	/* Enable all internal faults initially */
	fault_mgmt_arm_fmu_write32(dev, BIT_MASK(32), FAULT_MGMT_ARM_FMU_FIELD_SMEN);

	k_spin_unlock(&data->lock, key);

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

static void fault_mgmt_arm_fmu_isr(const struct device *dev, bool critical)
{
	uint32_t record_id, features;
	uint64_t errgsrs, lsb;
	bool is_critical, is_internal;
	struct fault_mgmt_arm_fmu_data *data = FAULT_MGMT_ARM_FMU_DEV_DATA(dev);
	k_spinlock_key_t key = k_spin_lock(&data->lock);

	errgsrs = fault_mgmt_arm_fmu_read32(dev, FAULT_MGMT_ARM_FMU_FIELD_ERRGSR);
	errgsrs |= ((uint64_t)fault_mgmt_arm_fmu_read32(dev, FAULT_MGMT_ARM_FMU_FIELD_ERRGSR2))
		   << 32;

	while (errgsrs) {
		lsb = LSB_GET(errgsrs);
		record_id = LOG2(lsb);
		features = fault_mgmt_arm_fmu_read32(dev, FAULT_MGMT_ARM_FMU_RECORD_FIELD_FR(record_id));
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

	k_spin_unlock(&data->lock, key);
}

static void fault_mgmt_arm_fmu_isr_critical(const struct device *dev)
{
	fault_mgmt_arm_fmu_isr(dev, true);
}

static void fault_mgmt_arm_fmu_isr_non_critical(const struct device *dev)
{
	fault_mgmt_arm_fmu_isr(dev, false);
}

static int fault_mgmt_arm_fmu_validate_prot_id(uint32_t prot_id)
{
	if (!IS_POWER_OF_TWO(prot_id)) {
		LOG_DBG("Unsupported protection ID: 0x%x\n", prot_id);
		return -EINVAL;
	}

	return 0;
}

static int fault_mgmt_arm_fmu_inject(const struct device *dev, uint32_t prot_id)
{
	int ret;
	uint32_t smerr;
	struct fault_mgmt_arm_fmu_data *data = FAULT_MGMT_ARM_FMU_DEV_DATA(dev);
	k_spinlock_key_t key = k_spin_lock(&data->lock);

	ret = fault_mgmt_arm_fmu_validate_prot_id(prot_id);
	if (ret < 0) {
		k_spin_unlock(&data->lock, key);
		return ret;
	}

	LOG_DBG("Injecting error 0x%x\n", prot_id);

	smerr = fault_mgmt_arm_fmu_read32(dev, FAULT_MGMT_ARM_FMU_FIELD_SMERR);
	fault_mgmt_arm_fmu_write32(dev, smerr | prot_id, FAULT_MGMT_ARM_FMU_FIELD_SMERR);

	k_spin_unlock(&data->lock, key);

	return 0;
}

static int fault_mgmt_arm_fmu_set_enabled(const struct device *dev, uint32_t prot_id, bool enabled)
{
	int ret;
	uint32_t smen;
	struct fault_mgmt_arm_fmu_data *data = FAULT_MGMT_ARM_FMU_DEV_DATA(dev);
	k_spinlock_key_t key = k_spin_lock(&data->lock);

	ret = fault_mgmt_arm_fmu_validate_prot_id(prot_id);
	if (ret < 0) {
		k_spin_unlock(&data->lock, key);
		return ret;
	}

	LOG_DBG("Changing enabled status of 0x%x\n", prot_id);

	smen = fault_mgmt_arm_fmu_read32(dev, FAULT_MGMT_ARM_FMU_FIELD_SMEN);
	smen = enabled ? smen | prot_id : smen & ~prot_id;
	fault_mgmt_arm_fmu_write32(dev, smen, FAULT_MGMT_ARM_FMU_FIELD_SMEN);

	k_spin_unlock(&data->lock, key);

	return 0;
}

static int fault_mgmt_arm_fmu_fault_callback_set(const struct device *dev,
						 fault_mgmt_arm_fmu_callback_t callback,
						 void *user_data)
{
	struct fault_mgmt_arm_fmu_data *data = FAULT_MGMT_ARM_FMU_DEV_DATA(dev);
	k_spinlock_key_t key = k_spin_lock(&data->lock);

	data->callback = callback;
	data->user_data = user_data;

	k_spin_unlock(&data->lock, key);

	return 0;
}

static const struct fault_mgmt_arm_fmu_api fault_mgmt_arm_fmu_system_api = {
	.inject = fault_mgmt_arm_fmu_inject,
	.set_enabled = fault_mgmt_arm_fmu_set_enabled,
	.fault_callback_set = fault_mgmt_arm_fmu_fault_callback_set,
};

#define DT_DRV_COMPAT arm_fmu

#define FAULT_MGMT_ARM_FMU_HAS_INTERRUPTS(n, if_true, if_false)                                    \
	COND_CODE_0(DT_NUM_IRQS(DT_DRV_INST(n)), if_false, if_true)

#define FAULT_MGMT_ARM_FMU_IRQ_CONFIG(n)                                                           \
	static void fault_mgmt_arm_fmu_irq_config_##n(const struct device *d)                      \
	{                                                                                          \
		IRQ_CONNECT(DT_INST_IRQ_BY_NAME(n, critical, irq),                                 \
			    DT_INST_IRQ_BY_NAME(n, critical, priority),                            \
			    fault_mgmt_arm_fmu_isr_critical, DEVICE_DT_INST_GET(n), 0);            \
		IRQ_CONNECT(DT_INST_IRQ_BY_NAME(n, non_critical, irq),                             \
			    DT_INST_IRQ_BY_NAME(n, non_critical, priority),                        \
			    fault_mgmt_arm_fmu_isr_non_critical, DEVICE_DT_INST_GET(n), 0);        \
		irq_enable(DT_INST_IRQ_BY_NAME(n, critical, irq));                                 \
		irq_enable(DT_INST_IRQ_BY_NAME(n, non_critical, irq));                             \
	}

#define FAULT_MGMT_ARM_FMU_INIT(n)                                                                 \
	FAULT_MGMT_ARM_FMU_HAS_INTERRUPTS(n, (FAULT_MGMT_ARM_FMU_IRQ_CONFIG(n)), ())               \
	static const struct fault_mgmt_arm_fmu_config fault_mgmt_arm_fmu_config_##n = {            \
		DEVICE_MMIO_ROM_INIT(DT_DRV_INST(n)),                                              \
		.irq_config = FAULT_MGMT_ARM_FMU_HAS_INTERRUPTS(                                   \
			n, (fault_mgmt_arm_fmu_irq_config_##n), (NULL)),                           \
	};                                                                                         \
	static struct fault_mgmt_arm_fmu_data fault_mgmt_arm_fmu_data_##n = {                      \
		.callback = NULL,                                                                  \
		.user_data = NULL,                                                                 \
	};                                                                                         \
	DEVICE_DT_INST_DEFINE(n, &fault_mgmt_arm_fmu_init, NULL, &fault_mgmt_arm_fmu_data_##n,     \
			      &fault_mgmt_arm_fmu_config_##n, POST_KERNEL,                         \
			      CONFIG_KERNEL_INIT_PRIORITY_DEVICE, &fault_mgmt_arm_fmu_system_api);

DT_INST_FOREACH_STATUS_OKAY(FAULT_MGMT_ARM_FMU_INIT);

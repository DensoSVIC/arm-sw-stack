/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(fault_mgmt_arm_fmu, CONFIG_FAULT_MGMT_LOG_LEVEL);

#include <zephyr/devicetree.h>
#include <zephyr/kernel.h>
#include <zephyr/spinlock.h>

#include "zephyr/drivers/fault_mgmt/fault_mgmt_arm_fmu.h"
#include "fault_mgmt_arm_fmu_priv.h"

static int fault_mgmt_arm_fmu_implementation_init(
	const struct device *dev, const struct fault_mgmt_arm_fmu_implementation *implementation)
{
	int ret = 0;
	struct fault_mgmt_arm_fmu_data *data = FAULT_MGMT_ARM_FMU_DEV_DATA(dev);
	k_spinlock_key_t key;

	if (implementation->init_fn) {
		key = k_spin_lock(&data->lock);
		ret = implementation->init_fn(dev);
		k_spin_unlock(&data->lock, key);
	}

	return ret;
}

static int fault_mgmt_arm_fmu_init(const struct device *dev)
{
	const struct fault_mgmt_arm_fmu_config *cfg = FAULT_MGMT_ARM_FMU_DEV_CFG(dev);
	struct fault_mgmt_arm_fmu_data *data = FAULT_MGMT_ARM_FMU_DEV_DATA(dev);
	uint32_t erriidr;
	int ret;

	DEVICE_MMIO_MAP(dev, K_MEM_CACHE_NONE);

	/* ERRIIDR is constant so no spinlock required */
	erriidr = fault_mgmt_arm_fmu_read32(dev, FAULT_MGMT_ARM_FMU_FIELD_ERRIIDR);
	STRUCT_SECTION_FOREACH(fault_mgmt_arm_fmu_implementation, implementation) {
		if (erriidr == implementation->erriidr) {
			ret = fault_mgmt_arm_fmu_implementation_init(dev, implementation);
			if (ret < 0) {
				return ret;
			}
			data->internal_api = implementation->api;
		}
	}

	if (!data->internal_api) {
		LOG_ERR("No FMU implementation found for 0x%x\n", erriidr);
		return -ENOTSUP;
	}

	if (cfg->irq_config) {
		cfg->irq_config(dev);
	}

	return 0;
}

static void fault_mgmt_arm_fmu_isr(const struct device *dev, bool critical)
{
	struct fault_mgmt_arm_fmu_data *data = FAULT_MGMT_ARM_FMU_DEV_DATA(dev);
	k_spinlock_key_t key;

	key = k_spin_lock(&data->lock);
	data->internal_api->isr(dev, critical);
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

static int fault_mgmt_arm_fmu_inject(const struct device *dev, uint32_t prot_id)
{
	int ret;
	struct fault_mgmt_arm_fmu_data *data = FAULT_MGMT_ARM_FMU_DEV_DATA(dev);
	k_spinlock_key_t key;

	key = k_spin_lock(&data->lock);
	ret = data->internal_api->inject(dev, prot_id);
	k_spin_unlock(&data->lock, key);

	return ret;
}

static int fault_mgmt_arm_fmu_set_enabled(const struct device *dev, uint32_t prot_id, bool enabled)
{
	int ret;
	struct fault_mgmt_arm_fmu_data *data = FAULT_MGMT_ARM_FMU_DEV_DATA(dev);
	k_spinlock_key_t key;

	key = k_spin_lock(&data->lock);
	ret = data->internal_api->set_enabled(dev, prot_id, enabled);
	k_spin_unlock(&data->lock, key);

	return ret;
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
		.internal_api = NULL,                                                              \
	};                                                                                         \
	DEVICE_DT_INST_DEFINE(n, &fault_mgmt_arm_fmu_init, NULL, &fault_mgmt_arm_fmu_data_##n,     \
			      &fault_mgmt_arm_fmu_config_##n, POST_KERNEL,                         \
			      CONFIG_KERNEL_INIT_PRIORITY_DEVICE, &fault_mgmt_arm_fmu_system_api);

DT_INST_FOREACH_STATUS_OKAY(FAULT_MGMT_ARM_FMU_INIT);

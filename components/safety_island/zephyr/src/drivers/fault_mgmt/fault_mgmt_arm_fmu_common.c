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

static void fault_mgmt_arm_fmu_isr(const struct device *root_dev, bool critical)
{
	int ret;
	uint32_t next_id;
	uint64_t num_iterations = 0;
	struct fault_mgmt_arm_fmu_data *data;
	struct fault_mgmt_arm_fmu_data *root_data = FAULT_MGMT_ARM_FMU_DEV_DATA(root_dev);
	struct fault_mgmt_arm_fmu_fault fault;
	struct stack_state {
		const struct device *dev;
		k_spinlock_key_t key;
	};
	struct stack_state stack[CONFIG_FAULT_MGMT_MAX_TREE_DEPTH + 1] = {{
		.dev = root_dev,
		.key = k_spin_lock(&root_data->lock),
	}};
	struct stack_state *stack_ptr = stack;

	__ASSERT(root_data->callback != NULL, "Fault received without callback");

	while (stack_ptr >= stack) {
		if (stack_ptr - stack >= CONFIG_FAULT_MGMT_MAX_TREE_DEPTH) {
			LOG_ERR("Maximum FMU tree depth exceeded\n");
			k_oops();
		}
		if (num_iterations >= FAULT_MGMT_ARM_FMU_MAX_FAULT_ITERATIONS) {
			LOG_ERR("Maximum FMU callback iterations exceeded\n");
			k_oops();
		}

		LOG_DBG("Next fault: %s, depth=%zd\n", stack_ptr->dev->name, stack_ptr - stack);
		next_id = 0;
		data = FAULT_MGMT_ARM_FMU_DEV_DATA(stack_ptr->dev);
		ret = data->internal_api->next_fault(stack_ptr->dev, critical, &next_id);
		LOG_DBG("Return: ret=0x%x, next_id=0x%x\n", ret, next_id);

		if (ret == FAULT_MGMT_ARM_FMU_NEXT_FAULT_UPSTREAM) {
			/* Next fault is from an upstream FMU
			 * Put next_id (the next device handle) on the stack
			 */
			stack_ptr++;
			stack_ptr->dev = device_from_handle(next_id);
			data = FAULT_MGMT_ARM_FMU_DEV_DATA(stack_ptr->dev);
			stack_ptr->key = k_spin_lock(&data->lock);
		} else if (ret == 0) {
			if (next_id == FAULT_MGMT_ARM_FMU_FAULT_PROTECTION_ID_INVALID) {
				/* No more errors so rewind the stack */
				k_spin_unlock(&data->lock, stack_ptr->key);
				stack_ptr--;
			} else {
				/* A fault has been reported - send to callback */
				fault.handle = device_handle_get(stack_ptr->dev);
				fault.prot_id = next_id;
				root_data->callback(root_dev, &fault, root_data->user_data);
			}
		} else {
			LOG_ERR("Invalid next_fault (ret=0x%x, next_id=0x%x)\n", ret, next_id);
			k_oops();
		}
		num_iterations++;
	}
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

static int fault_mgmt_arm_fmu_set_critical(const struct device *dev, uint32_t prot_id,
					   bool critical)
{
	int ret;
	struct fault_mgmt_arm_fmu_data *data = FAULT_MGMT_ARM_FMU_DEV_DATA(dev);
	k_spinlock_key_t key;

	key = k_spin_lock(&data->lock);
	ret = data->internal_api->set_critical(dev, prot_id, critical);
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
	.set_critical = fault_mgmt_arm_fmu_set_critical,
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

/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "zephyr/subsys/fault_mgmt/fault_mgmt.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(fault_mgmt, CONFIG_FAULT_MGMT_LOG_LEVEL);

#include <zephyr/init.h>
#include <zephyr/kernel.h>

#include "zephyr/drivers/fault_mgmt/fault_mgmt_arm_fmu.h"

#ifdef CONFIG_FAULT_MGMT_STORAGE
#include "fault_mgmt_priv.h"
#endif

/* Ensure all root FMUs have the "okay" status and have IRQs defined */
#define DT_DRV_COMPAT zephyr_fault_mgmt
BUILD_ASSERT(DT_NUM_INST_STATUS_OKAY(DT_DRV_COMPAT) == 1,
	     "There should be exactly one zephyr,fault-mgmt node");
#define DT_FAULT_MGMT DT_COMPAT_GET_ANY_STATUS_OKAY(DT_DRV_COMPAT)
#define BUILD_ASSERT_VALID(node_id, prop, idx)                                                     \
	BUILD_ASSERT(DT_NODE_HAS_STATUS(DT_PHANDLE_BY_IDX(node_id, prop, idx), okay),              \
		     "All root FMUs must have a status okay");                                     \
	BUILD_ASSERT(DT_NUM_IRQS(DT_PHANDLE_BY_IDX(node_id, prop, idx)) > 0,                       \
		     "All root FMUs must have IRQs");
DT_FOREACH_PROP_ELEM(DT_FAULT_MGMT, root_fmus, BUILD_ASSERT_VALID)

#define PHANDLE_TO_DEVICE(node_id, prop, idx) DEVICE_DT_GET(DT_PHANDLE_BY_IDX(node_id, prop, idx)),
static const struct device *fault_mgmt_root_fmus[] = {
	DT_FOREACH_PROP_ELEM(DT_FAULT_MGMT, root_fmus, PHANDLE_TO_DEVICE)};
BUILD_ASSERT(ARRAY_SIZE(fault_mgmt_root_fmus) > 0, "At least one root FMU must be defined");

/* Define separate message queues and threads for critical and non-critical
 * faults
 */
K_MSGQ_DEFINE(fault_mgmt_msgq_critical, sizeof(struct fault_mgmt_arm_fmu_fault),
	      CONFIG_FAULT_MGMT_QUEUE_SIZE_CRITICAL, 4);
K_THREAD_STACK_DEFINE(fault_mgmt_stack_critical, CONFIG_FAULT_MGMT_STACK_SIZE);
static struct k_thread fault_mgmt_thread_critical;

K_MSGQ_DEFINE(fault_mgmt_msgq_non_critical, sizeof(struct fault_mgmt_arm_fmu_fault),
	      CONFIG_FAULT_MGMT_QUEUE_SIZE_NON_CRITICAL, 4);
K_THREAD_STACK_DEFINE(fault_mgmt_stack_non_critical, CONFIG_FAULT_MGMT_STACK_SIZE);
static struct k_thread fault_mgmt_thread_non_critical;

static void fault_mgmt_fault_callback(const struct device *dev,
				      const struct fault_mgmt_arm_fmu_fault *fault, void *user_data)
{
	int ret;
	struct k_msgq *target_msgq = FAULT_MGMT_ARM_FMU_FAULT_IS_CRITICAL(fault)
					     ? &fault_mgmt_msgq_critical
					     : &fault_mgmt_msgq_non_critical;

	ret = k_msgq_put(target_msgq, fault, K_NO_WAIT);
	/* Abort if the queue has overflowed */
	if (ret < 0) {
		LOG_ERR("Failed to push fault to queue: 0x%x\n", ret);
		k_oops();
	}
}

static void fault_mgmt_handler(void *arg0, void *arg1, void *arg2)
{
	struct fault_mgmt_arm_fmu_fault fault;
	struct k_msgq *msgq = (struct k_msgq *)arg0;
	const struct device *dev;
	uint32_t protection_id;
	const char *criticality;

	ARG_UNUSED(arg1);
	ARG_UNUSED(arg2);

	while (1) {
		k_msgq_get(msgq, &fault, K_FOREVER);

		dev = device_from_handle(fault.handle);

		protection_id = FAULT_MGMT_ARM_FMU_FAULT_PROTECTION_ID(&fault);
		criticality =
			FAULT_MGMT_ARM_FMU_FAULT_IS_CRITICAL(&fault) ? "critical" : "non-critical";

#ifdef CONFIG_FAULT_MGMT_STORAGE
		uint64_t total_size = fault_mgmt_storage_write(&fault);

		LOG_INF("Fault received (%s): 0x%x on %s : count %llu\n", criticality,
			protection_id, dev->name, total_size);
#else
		LOG_INF("Fault received (%s): 0x%x on %s\n", criticality, protection_id, dev->name);
#endif
	}
}

int fault_mgmt_inject(const struct device *dev, uint32_t prot_id)
{
	return FAULT_MGMT_ARM_FMU_DEV_API(dev)->inject(dev, prot_id);
}

int fault_mgmt_set_enabled(const struct device *dev, uint32_t prot_id, bool enabled)
{
	return FAULT_MGMT_ARM_FMU_DEV_API(dev)->set_enabled(dev, prot_id, enabled);
}

int fault_mgmt_set_critical(const struct device *dev, uint32_t prot_id, bool critical)
{
	return FAULT_MGMT_ARM_FMU_DEV_API(dev)->set_critical(dev, prot_id, critical);
}

int fault_mgmt_device_foreach(fault_mgmt_device_callback callback, void *cookie)
{
	int ret;
	size_t i;
	device_handle_t root_fmu_handles[ARRAY_SIZE(fault_mgmt_root_fmus)];
	struct stack_state {
		const device_handle_t *fmus;
		size_t count;
		size_t index;
	};
	struct stack_state stack[CONFIG_FAULT_MGMT_MAX_TREE_DEPTH + 1] = {{
		.fmus = root_fmu_handles,
		.count = ARRAY_SIZE(root_fmu_handles),
		.index = 0,
	}};
	struct stack_state *stack_ptr = stack;
	const struct device *dev;

	for (i = 0; i < ARRAY_SIZE(root_fmu_handles); i++) {
		root_fmu_handles[i] = device_handle_get(fault_mgmt_root_fmus[i]);
	}

	while (stack_ptr >= stack) {
		if (stack_ptr->index >= stack_ptr->count) {
			stack_ptr--;
			continue;
		}

		dev = device_from_handle(stack_ptr->fmus[stack_ptr->index]);
		ret = callback(dev, stack_ptr - stack, stack_ptr->index, cookie);
		if (ret < 0) {
			return ret;
		}

		stack_ptr->index++;
		stack_ptr++;
		if (stack_ptr - stack > CONFIG_FAULT_MGMT_MAX_TREE_DEPTH) {
			LOG_ERR("FMU tree depth exceeds CONFIG_FAULT_MGMT_MAX_TREE_DEPTH\n");
			return -ENOMEM;
		}
		stack_ptr->fmus = device_required_handles_get(dev, &stack_ptr->count);
		stack_ptr->index = 0;
	}

	return 0;
}

static int fault_mgmt_validate_callback(const struct device *dev, size_t depth, size_t index,
					void *cookie)
{
	ARG_UNUSED(depth);
	ARG_UNUSED(index);
	ARG_UNUSED(cookie);

	if (!device_is_ready(dev)) {
		LOG_ERR("FMU %s is not ready\n", dev->name);
		return -ENODEV;
	}

	return 0;
}

static int fault_mgmt_prepare_root_fmus(void)
{
	int i;
	const struct device *dev;

	for (i = 0; i < ARRAY_SIZE(fault_mgmt_root_fmus); i++) {
		dev = fault_mgmt_root_fmus[i];
		FAULT_MGMT_ARM_FMU_DEV_API(dev)->fault_callback_set(dev, fault_mgmt_fault_callback,
								    NULL);

		LOG_DBG("Fault management initialized for root FMU: %s\n", dev->name);
	}

	return 0;
}

static int fault_mgmt_init(const struct device *dev)
{
	k_tid_t tid;
	int ret;

	ARG_UNUSED(dev);

	/* Ensure all FMUs in the tree are ready */
	ret = fault_mgmt_device_foreach(fault_mgmt_validate_callback, NULL);
	if (ret < 0) {
		return ret;
	}

	/* Attach root FMU callbacks */
	ret = fault_mgmt_prepare_root_fmus();
	if (ret < 0) {
		return ret;
	}

#ifdef CONFIG_FAULT_MGMT_PSA_PROTECTED_STORAGE
	fault_mgmt_storage_init_psa_protected_storage();
#endif

	/* Create critical fault thread */
	tid = k_thread_create(&fault_mgmt_thread_critical, fault_mgmt_stack_critical,
			      K_THREAD_STACK_SIZEOF(fault_mgmt_stack_critical), fault_mgmt_handler,
			      &fault_mgmt_msgq_critical, NULL, NULL,
			      K_PRIO_COOP(CONFIG_FAULT_MGMT_THREAD_PRIORITY_CRITICAL), 0,
			      K_NO_WAIT);
	k_thread_name_set(tid, "fault_mgmt_critical");

	/* Create non-critical fault thread */
	tid = k_thread_create(&fault_mgmt_thread_non_critical, fault_mgmt_stack_non_critical,
			      K_THREAD_STACK_SIZEOF(fault_mgmt_stack_non_critical),
			      fault_mgmt_handler, &fault_mgmt_msgq_non_critical, NULL, NULL,
			      K_PRIO_COOP(CONFIG_FAULT_MGMT_THREAD_PRIORITY_NON_CRITICAL), 0,
			      K_NO_WAIT);
	k_thread_name_set(tid, "fault_mgmt_non_critical");

	return 0;
}

DEVICE_DT_DEFINE(DT_FAULT_MGMT, fault_mgmt_init, NULL, NULL, NULL, APPLICATION,
		 CONFIG_APPLICATION_INIT_PRIORITY, NULL);

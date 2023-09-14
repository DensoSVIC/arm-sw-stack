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

/* Ensure all root FMUs have the "okay" status and have IRQs defined */
#define BUILD_ASSERT_VALID(node_id, prop, idx)                                                     \
	BUILD_ASSERT(DT_NODE_HAS_STATUS(DT_PHANDLE_BY_IDX(node_id, prop, idx), okay),              \
		     "All root FMUs must have a status okay");                                     \
	BUILD_ASSERT(DT_NUM_IRQS(DT_PHANDLE_BY_IDX(node_id, prop, idx)) > 0,                       \
		     "All root FMUs must have IRQs");
DT_FOREACH_PROP_ELEM(ZEPHYR_USER_NODE, root_fmus, BUILD_ASSERT_VALID)

#define PHANDLE_TO_DEVICE(node_id, prop, idx) DEVICE_DT_GET(DT_PHANDLE_BY_IDX(node_id, prop, idx)),
const struct device *fault_mgmt_root_fmus[] = {
	DT_FOREACH_PROP_ELEM(ZEPHYR_USER_NODE, root_fmus, PHANDLE_TO_DEVICE)};
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
	__ASSERT(ret == 0, "Failed to push fault to queue: 0x%x", ret);
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
		LOG_INF("Fault received (%s): 0x%x on %s\n", criticality, protection_id, dev->name);
	}
}

static int fault_mgmt_prepare_root_fmus(void)
{
	int i;
	const struct device *dev;
	const struct fault_mgmt_arm_fmu_api *api;

	for (i = 0; i < ARRAY_SIZE(fault_mgmt_root_fmus); i++) {
		dev = fault_mgmt_root_fmus[i];

		if (!device_is_ready(dev)) {
			LOG_ERR("Root FMU %s is not ready\n", dev->name);
			return -ENODEV;
		}

		api = dev->api;
		api->fault_callback_set(dev, fault_mgmt_fault_callback, NULL);

		LOG_DBG("Fault management initialized for root FMU: %s\n", dev->name);
	}

	return 0;
}

static int fault_mgmt_init(void)
{
	k_tid_t tid;
	int ret;

	ret = fault_mgmt_prepare_root_fmus();
	if (ret < 0) {
		return ret;
	}

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

SYS_INIT(fault_mgmt_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/hash_map.h>
#include <zephyr/sys/math_extras.h>

#include "zephyr/drivers/fault_mgmt/fault_mgmt_arm_fmu.h"
#include "zephyr/subsys/fault_mgmt/fault_mgmt_storage.h"
#include "fault_mgmt_priv.h"

LOG_MODULE_REGISTER(fault_mgmt_storage_sys_hash_map, CONFIG_FAULT_MGMT_LOG_LEVEL);

uint64_t fault_mgmt_storage_write(struct fault_mgmt_arm_fmu_fault *fault)
{
	uint64_t counter;
	uint64_t combined_key;
	bool inc_overflow;
	int ret;

	k_mutex_lock(&fault_mgmt_storage_mutex, K_FOREVER);
	combined_key = GENERATE_FMU_STORAGE_KEY(fault->handle, fault->prot_id);
	if (sys_hashmap_get(&fmu_fault_map, combined_key, &counter)) {
		inc_overflow = u64_add_overflow(counter, 1, &counter);
		if (inc_overflow) {
			LOG_ERR("%s: Incrementing counter caused overflow", __func__);
			k_oops();
		}
	} else {
		counter = 1;
	}

	ret = sys_hashmap_insert(&fmu_fault_map, combined_key, counter, NULL);
	if (ret < 0) {
		LOG_ERR("Failed to write log to storage");
		k_oops();
	}

	k_mutex_unlock(&fault_mgmt_storage_mutex);

	return counter;
}

void fault_mgmt_storage_clear(void)
{
	k_mutex_lock(&fault_mgmt_storage_mutex, K_FOREVER);
	sys_hashmap_clear(&fmu_fault_map, NULL, NULL);
	k_mutex_unlock(&fault_mgmt_storage_mutex);
}

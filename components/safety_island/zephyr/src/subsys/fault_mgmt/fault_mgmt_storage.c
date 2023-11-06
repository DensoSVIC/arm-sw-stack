/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/hash_map.h>

#include "zephyr/subsys/fault_mgmt/fault_mgmt_storage.h"
#include "fault_mgmt_priv.h"

SYS_HASHMAP_DEFINE(fmu_fault_map);
K_MUTEX_DEFINE(fault_mgmt_storage_mutex);

/**
 * A structure to store parameters for handling
 * fault iteration and callback.
 */
static struct fault_storage_list_params {
	fault_mgmt_storage_callback callback;
	uint64_t max_count;
} params;

/* Callback function to calculate total faults reported and find max count */
static void fault_mgmt_storage_fault_count_callback(uint64_t key, uint64_t count, void *cookie)
{
	fault_storage_stats_t *count_info = (fault_storage_stats_t *)cookie;

	count_info->total_fault += count;
	if (count > count_info->highest_count) {
		count_info->highest_count = count;
	}
}

/* Function to return total stored faults and the highest count */
void fault_mgmt_storage_stats(fault_storage_stats_t *stats)
{
	k_mutex_lock(&fault_mgmt_storage_mutex, K_FOREVER);
	sys_hashmap_foreach(&fmu_fault_map, fault_mgmt_storage_fault_count_callback, stats);
	k_mutex_unlock(&fault_mgmt_storage_mutex);
}

/* Callback function to list faults based on a minimum count threshold */
static void fault_mgmt_storage_list_conditional_cb(uint64_t key, uint64_t count, void *cookie)
{
	struct fault_mgmt_storage_info fault_storage;
	uint32_t handle = FIELD_GET(~BIT64_MASK(32), key);
	uint32_t prot_id = FIELD_GET(BIT64_MASK(32), key);

	if (count >= params.max_count) {
		fault_storage.fault.handle = handle;
		fault_storage.fault.prot_id = prot_id;
		fault_storage.count = count;

		params.callback(&fault_storage, cookie);
	}
}

void fault_mgmt_storage_foreach(fault_mgmt_storage_callback callback, uint64_t threshold,
				void *cookie)
{
	k_mutex_lock(&fault_mgmt_storage_mutex, K_FOREVER);
	params.callback = callback;
	params.max_count = threshold;
	sys_hashmap_foreach(&fmu_fault_map, fault_mgmt_storage_list_conditional_cb, cookie);
	k_mutex_unlock(&fault_mgmt_storage_mutex);
}


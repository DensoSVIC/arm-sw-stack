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

#define COMBINE64(handle, prot_id) ((uint64_t)(handle) << 32 | (prot_id))
#define BIT_MASK_32                0xFFFFFFFF

LOG_MODULE_REGISTER(fault_mgmt_storage, CONFIG_FAULT_MGMT_LOG_LEVEL);

/* Define hashmap to store error details and count */
SYS_HASHMAP_DEFINE_STATIC(fmu_error_map);
K_MUTEX_DEFINE(fault_mgmt_storage_mutex);

/**
 * A structure to store parameters for handling
 * fault iteration and callback.
 */
static struct fault_storage_list_params {
	fault_mgmt_storage_callback callback;
	uint64_t max_count;
} params;

uint64_t fault_mgmt_storage_write(struct fault_mgmt_arm_fmu_fault *fault)
{
	uint64_t counter;
	uint64_t combined_key;
	bool inc_overflow;
	int ret;

	k_mutex_lock(&fault_mgmt_storage_mutex, K_FOREVER);
	combined_key = COMBINE64(fault->handle, fault->prot_id);
	if (sys_hashmap_get(&fmu_error_map, combined_key, &counter)) {
		inc_overflow = u64_add_overflow(counter, 1, &counter);
		if (inc_overflow) {
			LOG_ERR("%s: Incrementing counter caused overflow", __func__);
			k_oops();
		}
	} else {
		counter = 1;
	}

	ret = sys_hashmap_insert(&fmu_error_map, combined_key, counter, NULL);
	if (ret < 0) {
		LOG_ERR("Failed to write log to storage");
		k_oops();
	}

	k_mutex_unlock(&fault_mgmt_storage_mutex);

	return counter;
}

/* Callback function to find most repeated */
static void fault_mgmt_storage_find_max_iterate_cb(uint64_t key, uint64_t count, void *cookie)
{
	uint64_t *max_count = (uint64_t *)cookie;

	if (count > *max_count) {
		*max_count = count;
	}
}

/* Callback function to print all entry's */
static void fault_mgmt_storage_list_most_iterated_cb(uint64_t key, uint64_t count, void *cookie)
{
	struct fault_mgmt_storage_info fault_storage;
	uint32_t handle, prot_id;

	if (count == params.max_count) {
		handle = (uint32_t)FIELD_GET(~BIT64_MASK(32), key);
		prot_id = FIELD_GET(BIT_MASK_32, key);
		fault_storage.fault.handle = handle;
		fault_storage.fault.prot_id = prot_id;
		fault_storage.count = count;
		params.callback(&fault_storage, cookie);
	}
}

/* Callback function to calculate total faults reported */
static void fault_mgmt_storage_total_cb(uint64_t key, uint64_t count, void *cookie)
{
	uint64_t *total_count = (uint64_t *)cookie;
	bool inc_overflow = u64_add_overflow(*total_count, count, total_count);

	if (inc_overflow) {
		LOG_ERR("%s: Incrementing counter caused overflow", __func__);
		k_oops();
	}
}

uint64_t fault_mgmt_storage_total_fault_reported(void)
{
	uint64_t total_count = 0;

	sys_hashmap_foreach(&fmu_error_map, fault_mgmt_storage_total_cb, &total_count);

	return total_count;
}

static void fault_mgmt_storage_list_cb(uint64_t key, uint64_t count, void *cookie)
{
	struct fault_mgmt_storage_info fault_storage;
	uint32_t handle = FIELD_GET(~BIT64_MASK(32), key);
	uint32_t prot_id = FIELD_GET(BIT_MASK_32, key);

	fault_storage.fault.handle = handle;
	fault_storage.fault.prot_id = prot_id;
	fault_storage.count = count;

	params.callback(&fault_storage, cookie);
}

void fault_mgmt_storage_foreach(fault_mgmt_storage_callback callback, void *cookie, int option)
{
	params.callback = callback;

	if (option == FAULT_MGMT_OPTION_LIST_FAULT) {
		sys_hashmap_foreach(&fmu_error_map, fault_mgmt_storage_list_cb, cookie);
	} else if (option == FAULT_MGMT_OPTION_LIST_MOST_REPORTED_FAULT) {
		uint64_t max_count = 0;

		/* Iterate through the hashmap to find the maximum iterate count */
		sys_hashmap_foreach(&fmu_error_map, fault_mgmt_storage_find_max_iterate_cb,
				    &max_count);
		/* If there is no repetitions, no need to proceed */
		if (max_count < 2) {
			return;
		}

		params.max_count = max_count;
		sys_hashmap_foreach(&fmu_error_map, fault_mgmt_storage_list_most_iterated_cb,
				    cookie);
	} else {
		__ASSERT(0, "Invalid option");
	}
}

void fault_mgmt_storage_clear(void)
{
	sys_hashmap_clear(&fmu_error_map, NULL, NULL);
}

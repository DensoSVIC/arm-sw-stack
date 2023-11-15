/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/math_extras.h>
#include <psa/protected_storage.h>
#include <zephyr/sys/hash_map.h>

#include "zephyr/drivers/fault_mgmt/fault_mgmt_device.h"
#include "zephyr/subsys/fault_mgmt/fault_mgmt_storage.h"
#include "fault_mgmt_priv.h"

#define FAULT_MGMT_STATIC_UID CONFIG_FAULT_MGMT_NAMESPACE_ID

LOG_MODULE_REGISTER(fault_mgmt_protected_storage, CONFIG_FAULT_MGMT_LOG_LEVEL);

static struct hashmap_ps_entry {
	uint64_t key;
	uint64_t value;
} ps_storage_map[CONFIG_MAX_PSA_PROTECTED_STORAGE_SIZE];

static size_t hash_map_index;

static void hashmap_to_serialize_callback(uint64_t key, uint64_t value, void *arg)
{
	if (hash_map_index < CONFIG_MAX_PSA_PROTECTED_STORAGE_SIZE) {
		ps_storage_map[hash_map_index].key = key;
		ps_storage_map[hash_map_index].value = value;
		hash_map_index++;
	} else {
		LOG_ERR("%s: Incorrect hash_map_index\n", __func__);
		k_oops();
	}
}

static void fault_mgmt_storage_serialize(void)
{
	memset(ps_storage_map, 0, sizeof(ps_storage_map));
	hash_map_index = 0;
	sys_hashmap_foreach(&fault_map, hashmap_to_serialize_callback, NULL);
}

static void fault_mgmt_storage_deserialize(void)
{
	for (int idx = 0;
	     idx < CONFIG_MAX_PSA_PROTECTED_STORAGE_SIZE && ps_storage_map[idx].key != 0; idx++) {
		sys_hashmap_insert(&fault_map, ps_storage_map[idx].key, ps_storage_map[idx].value,
				   NULL);
	}
}

void fault_mgmt_storage_init_psa_protected_storage(void)
{
	size_t data_size;
	psa_status_t status;
	const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(psa));

	if (!device_is_ready(dev)) {
		LOG_ERR("PSA device is not ready\n");
		k_oops();
	}

	memset(ps_storage_map, 0, sizeof(ps_storage_map));
	status = psa_ps_get(FAULT_MGMT_STATIC_UID, 0, sizeof(ps_storage_map), &ps_storage_map,
			    &data_size);

	if (status == PSA_SUCCESS) {
		if (data_size == sizeof(ps_storage_map)) {
			fault_mgmt_storage_deserialize();
		} else {
			LOG_ERR("%s: Data size mismatch in PSA Protected Storage", __func__);
		}
	} else if (status != PSA_ERROR_DOES_NOT_EXIST) {
		LOG_ERR("%s: Error accessing PSA Protected Storage - Status: %d", __func__, status);
		k_oops();
	}
	/* No action needed if status is PSA_ERROR_DOES_NOT_EXIST at this point */
}

uint64_t fault_mgmt_storage_write(struct fault_mgmt_fault *fault)
{
	psa_status_t status;
	uint64_t counter;
	uint64_t combined_key;
	bool inc_overflow;
	size_t storage_size;
	int ret;

	k_mutex_lock(&fault_mgmt_storage_mutex, K_FOREVER);
	combined_key = GENERATE_FAULT_STORAGE_KEY(fault->handle, fault->prot_id);
	if (sys_hashmap_get(&fault_map, combined_key, &counter)) {
		inc_overflow = u64_add_overflow(counter, 1, &counter);
		if (inc_overflow) {
			LOG_ERR("%s: Incrementing counter caused overflow", __func__);
			k_oops();
		}
	} else {
		storage_size = sys_hashmap_size(&fault_map);
		if (storage_size >= CONFIG_MAX_PSA_PROTECTED_STORAGE_SIZE) {
			LOG_ERR("PSA Protected Storage capacity reached");
			k_oops();
		}
		counter = 1;
	}

	ret = sys_hashmap_insert(&fault_map, combined_key, counter, NULL);
	if (ret < 0) {
		LOG_ERR("Failed to write log to storage");
		k_oops();
	}

	fault_mgmt_storage_serialize();
	status = psa_ps_set(FAULT_MGMT_STATIC_UID, sizeof(ps_storage_map), &ps_storage_map, 0);
	if (status != PSA_SUCCESS) {
		LOG_ERR("%s: PSA Protected Storage write operation failed: status %d\n", __func__,
			status);
		k_oops();
	}
	k_mutex_unlock(&fault_mgmt_storage_mutex);

	return counter;
}

void fault_mgmt_storage_clear(void)
{
	psa_status_t status;

	k_mutex_lock(&fault_mgmt_storage_mutex, K_FOREVER);
	status = psa_ps_remove(FAULT_MGMT_STATIC_UID);
	if (status == PSA_SUCCESS || status == PSA_ERROR_DOES_NOT_EXIST) {
		sys_hashmap_clear(&fault_map, NULL, NULL);
	} else {
		LOG_ERR("%s: Failed to clear UID records: status: %d\n", __func__, status);
		k_oops();
	}
	k_mutex_unlock(&fault_mgmt_storage_mutex);
}

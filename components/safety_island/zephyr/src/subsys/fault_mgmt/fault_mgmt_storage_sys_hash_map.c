/*
 * SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/hash_map.h>
#include <zephyr/sys/math_extras.h>

#include "zephyr/drivers/fault_mgmt/fault_mgmt_device.h"
#include "zephyr/subsys/fault_mgmt/fault_mgmt.h"
#include "zephyr/subsys/fault_mgmt/fault_mgmt_storage.h"
#include "fault_mgmt_priv.h"

LOG_MODULE_REGISTER(fault_mgmt_storage_sys_hash_map, CONFIG_FAULT_MGMT_LOG_LEVEL);

static void fault_mgmt_storage_write(const struct device *root_dev,
				     const struct fault_mgmt_fault *fault)
{
	uint64_t counter;
	uint64_t combined_key;
	bool inc_overflow;
	int ret;
	uint32_t protection_id;
	const struct device *dev;

	k_mutex_lock(&fault_mgmt_storage_mutex, K_FOREVER);
	combined_key = GENERATE_FAULT_STORAGE_KEY(fault->handle, fault->prot_id);
	if (sys_hashmap_get(&fault_map, combined_key, &counter)) {
		inc_overflow = u64_add_overflow(counter, 1, &counter);
		if (inc_overflow) {
			LOG_ERR("%s: Incrementing counter caused overflow", __func__);
			k_oops();
		}
	} else {
		counter = 1;
	}

	ret = sys_hashmap_insert(&fault_map, combined_key, counter, NULL);
	if (ret < 0) {
		LOG_ERR("Failed to write log to storage");
		k_oops();
	}

	k_mutex_unlock(&fault_mgmt_storage_mutex);

	protection_id = FAULT_MGMT_FAULT_PROTECTION_ID(fault);
	dev = device_from_handle(fault->handle);
	LOG_INF("Fault count for 0x%x on %s: %llu\n", protection_id, dev->name, counter);
}

void fault_mgmt_storage_clear(void)
{
	k_mutex_lock(&fault_mgmt_storage_mutex, K_FOREVER);
	sys_hashmap_clear(&fault_map, NULL, NULL);
	k_mutex_unlock(&fault_mgmt_storage_mutex);
}

FAULT_MGMT_HANDLER_DEFINE(CONFIG_FAULT_MGMT_STORAGE_PRIORITY, NULL, fault_mgmt_storage_write);

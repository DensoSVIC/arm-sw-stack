/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FAULT_MGMT_STORAGE_H_
#define FAULT_MGMT_STORAGE_H_

#include "zephyr/drivers/fault_mgmt/fault_mgmt_arm_fmu.h"

struct fault_mgmt_storage_info {
	struct fault_mgmt_arm_fmu_fault fault;
	uint64_t count;
};

typedef struct {
    uint64_t total_fault;
    uint64_t highest_count;
} fault_storage_stats_t;

typedef void (*fault_mgmt_storage_callback)(const struct fault_mgmt_storage_info *fault_info,
					    void *cookie);

/**
 * @brief To read the total number of faults reported and the highest count of a single fault repeated.
 *
 * This function iterates through the storage and accumulates the count values of all fault entries.
 * @param stats total and highest count
 */
void fault_mgmt_storage_stats(fault_storage_stats_t *stats);

/**
 * @brief A wrapper function on top of sys_hashmap_foreach() with option to select
 *
 * list reported fault based on the threshold given
 *
 * @param callback A call back function
 * @param threshold threshold to list faults based on repetition count
 * @param cookie user data
 */
void fault_mgmt_storage_foreach(fault_mgmt_storage_callback callback, uint64_t threshold, void *cookie);

/**
 * Clear all fault entries from the storage.
 */
void fault_mgmt_storage_clear(void);

#endif /* FAULT_MGMT_STORAGE_H_ */

/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 *
 * @brief Fault management storage API
 */

#ifndef FAULT_MGMT_STORAGE_H_
#define FAULT_MGMT_STORAGE_H_

#include "zephyr/drivers/fault_mgmt/fault_mgmt_device.h"

/**
 * @defgroup fault_mgmt_storage Storage
 * @ingroup fault_mgmt
 *
 * @brief Fault management storage component
 * @{
 */

/**
 * @brief Information about a stored fault
 *
 * Populated by @ref fault_mgmt_storage_foreach and passed to @ref fault_mgmt_storage_callback
 */
struct fault_mgmt_storage_info {
	/** @brief Fault metadata */
	struct fault_mgmt_fault fault;
	/** @brief The number of occurrences of the fault */
	uint64_t count;
};

/**
 * @brief A container of fault storage statistics
 *
 * Populated by @ref fault_mgmt_storage_stats
 */
typedef struct {
	/** @brief The total number of faults */
	uint64_t total_fault;
	/** @brief The highest fault count */
	uint64_t highest_count;
} fault_storage_stats_t;

/**
 * @brief The callback type of @ref fault_mgmt_storage_foreach
 *
 * @param fault_info Information about the stored fault
 * @param cookie     User-specific context
 */
typedef void (*fault_mgmt_storage_callback)(const struct fault_mgmt_storage_info *fault_info,
					    void *cookie);

/**
 * @brief To read the total number of faults reported and the highest count of a single fault
 * repeated.
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
void fault_mgmt_storage_foreach(fault_mgmt_storage_callback callback, uint64_t threshold,
				void *cookie);

/**
 * @brief Clear all fault entries from the storage.
 */
void fault_mgmt_storage_clear(void);

/** @} */

#endif /* FAULT_MGMT_STORAGE_H_ */

/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FAULT_MGMT_STORAGE_H_
#define FAULT_MGMT_STORAGE_H_

#include "zephyr/drivers/fault_mgmt/fault_mgmt_arm_fmu.h"

#define FAULT_MGMT_OPTION_LIST_FAULT               1
#define FAULT_MGMT_OPTION_LIST_MOST_REPORTED_FAULT 2

struct fault_mgmt_storage_info {
	struct fault_mgmt_arm_fmu_fault fault;
	uint32_t count;
};

typedef void (*fault_mgmt_storage_callback)(const struct fault_mgmt_storage_info *fault_info,
					    void *cookie);

/**
 * @brief Calculate the total count of fault entries reported.
 *
 * This function iterates through the storage and accumulates the count values of all fault entries.
 *
 * @return  total count of fault entries has been stored
 */
uint64_t fault_mgmt_storage_total_fault_reported(void);

/**
 * @brief A wrapper function on top of sys_hashmap_foreach() with option to select
 *
 * list all reported fault or Most reported fault
 *
 * @param callback A call back function
 * @param cookie user data
 * @param option user option to select operation
 */
void fault_mgmt_storage_foreach(fault_mgmt_storage_callback callback, void *cookie, int option);

/**
 * Clear all fault entries from the storage.
 */
void fault_mgmt_storage_clear(void);

#endif /* FAULT_MGMT_STORAGE_H_ */

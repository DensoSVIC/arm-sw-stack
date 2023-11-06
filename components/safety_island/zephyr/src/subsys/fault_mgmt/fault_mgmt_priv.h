/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FAULT_MGMT_PRIV_H_
#define FAULT_MGMT_PRIV_H_

#include <zephyr/kernel.h>
#include <zephyr/sys/hash_map.h>
#include "zephyr/drivers/fault_mgmt/fault_mgmt_arm_fmu.h"

#define GENERATE_FMU_STORAGE_KEY(handle, prot_id) ((uint64_t)(handle) << 32 | (prot_id))
extern struct sys_hashmap fmu_fault_map;
extern struct k_mutex fault_mgmt_storage_mutex;

/**
 * @brief Write fault data to the storage and increment the count for the given fault.
 *
 * @param fault A pointer to the fault data structure to be written.
 * @return The updated count for the given fault.
 */
uint64_t fault_mgmt_storage_write(struct fault_mgmt_arm_fmu_fault *fault);

#endif /* FAULT_MGMT_PRIV_H_ */

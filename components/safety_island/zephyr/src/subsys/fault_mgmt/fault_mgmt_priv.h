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

#define GENERATE_FAULT_STORAGE_KEY(handle, prot_id) ((uint64_t)(handle) << 32 | (prot_id))
extern struct sys_hashmap fault_map;
extern struct k_mutex fault_mgmt_storage_mutex;

#endif /* FAULT_MGMT_PRIV_H_ */

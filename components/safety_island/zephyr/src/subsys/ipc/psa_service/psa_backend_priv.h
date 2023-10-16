/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PSA_BACKEND_PRIV_H_
#define PSA_BACKEND_PRIV_H_

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/mbox.h>

struct psa_backend_data_t {
	struct k_work mbox_work;
	struct k_poll_signal receive_signal;
	struct k_poll_event receive_event[1U];
};

struct psa_backend_config_t {
	uintptr_t tx_shm_addr;
	uintptr_t rx_shm_addr;
	size_t tx_shm_size;
	size_t rx_shm_size;
	struct mbox_channel mbox_tx;
	struct mbox_channel mbox_rx;
};

#endif /* PSA_BACKEND_PRIV_H_ */

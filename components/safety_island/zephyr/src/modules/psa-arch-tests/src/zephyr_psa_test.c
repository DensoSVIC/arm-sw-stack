/*
 * Based on: https://github.com/zephyrproject-rtos/zephyr/blob/main/modules/trusted-firmware-m/src/zephyr_tfm_psa_test.c
 * In open-source project: zephyrproject-rtos/zephyr
 *
 * Original file: SPDX-FileCopyrightText: <text>Copyright 2021 Nordic Semiconductor ASA</text>
 * Modifications: SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited
 * and/or its affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Changes: None
 */

#include <zephyr/kernel.h>

/**
 * \brief This symbol is the entry point provided by the PSA API compliance
 *        test libraries
 */
extern void val_entry(void);


void psa_test(void)
{
	val_entry();
}

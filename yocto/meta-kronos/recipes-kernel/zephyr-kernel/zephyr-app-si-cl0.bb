#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

LICENSE = "MIT"

SUMMARY = "zephyr sample application for si_cl0"
DESCRIPTION = "A recipe can set the zephyr sample application on \
fvp_rd_kronos_cortex_r82_c0"

ZEPHYR_BOARD = "fvp_rd_kronos_cortex_r82_c0"
ZEPHYR_APP_SAFETY_ISLAND_CL0 ??= "helloworld"
ZEPHYR_APP = "${ZEPHYR_APP_SAFETY_ISLAND_CL0}"

include zephyr-app-si.inc

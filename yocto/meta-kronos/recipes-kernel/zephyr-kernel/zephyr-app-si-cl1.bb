#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

LICENSE = "MIT"

SUMMARY = "zephyr sample application for si_cl1"
DESCRIPTION = "A recipe can set the zephyr sample application on \
fvp_rdn2_automotive_cortex_r82_c1"

ZEPHYR_BOARD = "fvp_rd_kronos_cortex_r82_c1"
ZEPHYR_APP_SAFETY_ISLAND_CL1 ??= "synchronization"
ZEPHYR_APP = "${ZEPHYR_APP_SAFETY_ISLAND_CL1}"

include zephyr-fvp-rd-kronos.inc

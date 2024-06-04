# nooelint: oelint.var.mandatoryvar - The SRC_URI is found in
# a common .inc file in meta-zephyr.
#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "zephyr sample application for si_cl0"
DESCRIPTION = "A recipe can set the zephyr sample application on \
fvp_rd_kronos_safety_island_c0"
HOMEPAGE = "https://kronos-ref-stack.docs.arm.com/"
LICENSE = "MIT"

ZEPHYR_BOARD = "fvp_rd_kronos_safety_island_c0"
ZEPHYR_APP_SAFETY_ISLAND_CL0 ??= "helloworld"
ZEPHYR_APP = "${ZEPHYR_APP_SAFETY_ISLAND_CL0}"

require zephyr-app-si.inc

# nooelint: oelint.var.mandatoryvar - The SRC_URI is found in
# a common .inc file in meta-zephyr.
#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "zephyr sample application for si_cl1"
DESCRIPTION = "A recipe can set the zephyr sample application on \
fvp_rd_kronos_safety_island_c1"
HOMEPAGE = "https://kronos.docs.arm.com/"
LICENSE = "MIT"

ZEPHYR_BOARD = "fvp_rd_kronos_safety_island_c1"
ZEPHYR_APP_SAFETY_ISLAND_CL1 ??= "helloworld"
ZEPHYR_APP = "${ZEPHYR_APP_SAFETY_ISLAND_CL1}"

require zephyr-app-si.inc

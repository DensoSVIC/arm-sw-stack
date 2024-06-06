# nooelint: oelint.var.mandatoryvar - The SRC_URI is found in
# a common .inc file in meta-zephyr.
#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "zephyr sample application for si_cl0"
DESCRIPTION = "A recipe can set the zephyr sample application on \
Safety Island Cluster 0"
HOMEPAGE = "https://arm-auto-solutions.docs.arm.com/"
LICENSE = "MIT"

ZEPHYR_BOARD = "${MACHINE}_safety_island_c0"
ZEPHYR_APP_SAFETY_ISLAND_CL0 ??= "helloworld"
ZEPHYR_APP = "${ZEPHYR_APP_SAFETY_ISLAND_CL0}"

require zephyr-app-si.inc

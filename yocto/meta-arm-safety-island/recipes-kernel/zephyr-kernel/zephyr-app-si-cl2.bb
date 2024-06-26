# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "zephyr sample application for si_cl2"
DESCRIPTION = "A recipe can set the zephyr sample application on \
Safety Island Cluster 2"
HOMEPAGE = "https://arm-auto-solutions.docs.arm.com/"
LICENSE = "MIT"

ZEPHYR_BOARD = "${MACHINE}_safety_island_c2"
ZEPHYR_APP_SAFETY_ISLAND_CL2 ??= "helloworld"
ZEPHYR_APP = "${ZEPHYR_APP_SAFETY_ISLAND_CL2}"

require zephyr-app-si.inc

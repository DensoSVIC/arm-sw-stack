# nooelint: oelint.var.mandatoryvar - The SRC_URI is found in a common .inc file
# in meta-zephyr.
#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "Zephyr fault management application"
DESCRIPTION = "The Zephyr fault management application demonstrates the usage \
of Arm FMU devices using the shell."
HOMEPAGE = "https://kronos.docs.arm.com/"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://license.rst;md5=e805dc5353977631b7881c7705a6c04a"

require recipes-kernel/zephyr-kernel/zephyr-sample.inc

ZEPHYR_SRC_DIR = "${ZEPHYR_SAFETY_ISLAND_MODULE}/apps/fault_mgmt"

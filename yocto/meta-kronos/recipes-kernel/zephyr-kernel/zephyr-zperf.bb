#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

LICENSE = "MIT"

SUMMARY = "Zephyr zperf sample app"
DESCRIPTION = "A Zephyr application to test the network stack"

require recipes-kernel/zephyr-kernel/zephyr-sample.inc

ZEPHYR_SRC_DIR = "${ZEPHYR_BASE}/samples/net/zperf"

EXTRA_OECMAKE += "-DCONFIG_INIT_STACKS=n"

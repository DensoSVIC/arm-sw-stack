# nooelint: oelint.var.mandatoryvar - The SRC_URI is found in
# a common .inc file in meta-zephyr.
#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "Zephyr zperf sample app"
DESCRIPTION = "A Zephyr application to test the network stack"
HOMEPAGE = "https://docs.zephyrproject.org/${PREFERRED_VERSION_zephyr-kernel}/connectivity/networking/api/zperf.html"
LICENSE = "MIT"

require recipes-kernel/zephyr-kernel/zephyr-sample.inc

ZEPHYR_SRC_DIR = "${ZEPHYR_BASE}/samples/net/zperf"

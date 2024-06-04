# nooelint: oelint.var.mandatoryvar - The SRC_URI is found in a common .inc file
# in meta-zephyr.
#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "Run PSA Crypto Architecture Test Suite on Zephyr Mbedtls"
DESCRIPTION = "Run the PSA Crypto API compliance test suite to verify \
all PSA Crypto APIs implemented by Zephyr Mbedtls Module"
HOMEPAGE = "https://github.com/ARM-software/psa-arch-tests"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://license.rst;md5=e805dc5353977631b7881c7705a6c04a"

require recipes-kernel/zephyr-kernel/zephyr-sample.inc

ZEPHYR_SRC_DIR = "${ZEPHYR_SAFETY_ISLAND_MODULE}/apps/psa-crypto-tests"

OVERLAYS_PSA_BASENAME = "${ZEPHYR_SAFETY_ISLAND_MODULE}/overlays/psa/${ZEPHYR_BOARD}"
EXTRA_OECMAKE:append = "\
    -DDTC_OVERLAY_FILE='${OVERLAYS_PSA_BASENAME}.overlay' \
    -DOVERLAY_CONFIG='${OVERLAYS_PSA_BASENAME}.conf' \
"

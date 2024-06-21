# nooelint: oelint.var.mandatoryvar - The SRC_URI is found in a common .inc file
# in meta-zephyr.
#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "Zephyr PSA Secure Storage APIs Architecture Test Suite"
DESCRIPTION = "The architecture Secure Storage test suite is a set of examples \
of the invariant behaviors that are specified in the PSA Secure Storage APIs specifications."
HOMEPAGE = "https://github.com/ARM-software/psa-arch-tests"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://license.rst;md5=e805dc5353977631b7881c7705a6c04a"

require recipes-kernel/zephyr-kernel/zephyr-sample.inc

ZEPHYR_SRC_DIR = "${ZEPHYR_BASE}/samples/tfm_integration/tfm_psa_test"

OVERLAYS_PSA_BASENAME = "${ZEPHYR_SAFETY_ISLAND_MODULE}/overlays/psa/${BOARD}"
OVERLAYS_PSA_STORAGE_TESTS_BASENAME = "${ZEPHYR_SAFETY_ISLAND_MODULE}/apps/psa-storage-tests/boards/${BOARD}"
EXTRA_OECMAKE:append = "\
    -DDTC_OVERLAY_FILE='${OVERLAYS_PSA_BASENAME}.overlay' \
    -DOVERLAY_CONFIG='${OVERLAYS_PSA_BASENAME}.conf;${OVERLAYS_PSA_STORAGE_TESTS_BASENAME}.conf' \
"

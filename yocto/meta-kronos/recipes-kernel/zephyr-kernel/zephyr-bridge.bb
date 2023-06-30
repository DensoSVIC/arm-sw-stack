# nooelint: oelint.var.mandatoryvar - The SRC_URI is found in a common .inc file
# in meta-zephyr.
#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "Zephyr Bridge application"
DESCRIPTION = "The Zephyr Bridge application bridges the interfaces internal \
to Kronos with the interface to its external connection."
HOMEPAGE = "https://kronos.docs.arm.com/"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://license.rst;md5=e805dc5353977631b7881c7705a6c04a"

require recipes-kernel/zephyr-kernel/zephyr-sample.inc

ZEPHYR_SRC_DIR = "${ZEPHYR_SAFETY_ISLAND_MODULE}/apps/bridge"

OVERLAYS_HIPC_BASENAME = "${ZEPHYR_SAFETY_ISLAND_MODULE}/overlays/hipc/${ZEPHYR_BOARD}"
OVERLAYS_ETH0_BASENAME = "${ZEPHYR_SAFETY_ISLAND_MODULE}/overlays/ethernet0/${ZEPHYR_BOARD}"
EXTRA_OECMAKE:append = "\
    -DDTC_OVERLAY_FILE='${OVERLAYS_HIPC_BASENAME}.overlay;${OVERLAYS_ETH0_BASENAME}.overlay' \
    -DOVERLAY_CONFIG='${OVERLAYS_HIPC_BASENAME}.conf;${OVERLAYS_ETH0_BASENAME}.conf' \
"

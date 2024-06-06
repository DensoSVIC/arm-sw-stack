#
# SPDX-FileCopyrightText: <text>Copyright 2022-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "Actuation Service Zephyr application"
DESCRIPTION = "The Actuation Service demo showcases the Pure Pursuit algorithm \
from Autoware.Auto running as a Zephyr application."
HOMEPAGE = "https://safety-island-actuation-demo.docs.arm.com/"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://license.rst;md5=e805dc5353977631b7881c7705a6c04a"

require recipes-kernel/zephyr-kernel/zephyr-sample.inc
require recipes-demos/actuation/actuation-common.inc
require recipes-demos/actuation/cyclonedds_0.10.3.inc

DEPENDS += "\
    ament-cmake-auto-native \
    ament-cmake-python-native \
    ament-cmake-target-dependencies-native \
    cyclonedds-native \
    libeigen-native \
"

EXTRA_OECMAKE:append = "\
    -DCONTROL_CMDS_FWD=bsd_socket \
"

SRC_URI:append = " ${SRC_URI_ACTUATION};${BRANCH_ACTUATION};name=actuation;destsuffix=git/modules/lib/actuation"
SRC_URI:append = " ${SRC_URI_CYCLONEDDS};name=cyclonedds;destsuffix=git/modules/lib/actuation/cyclonedds"

SRCREV_actuation = "${SRCREV_ACTUATION}"
SRCREV_cyclonedds = "${SRCREV_CYCLONEDDS}"

ZEPHYR_SRC_DIR = "${S}/modules/lib/actuation/zephyr_app"

OVERLAY_BASENAME_FILE = "${ZEPHYR_SAFETY_ISLAND_MODULE}/overlays/hipc/${BOARD}"
OVERLAY_ACTUATION_BASENAME_FILE = "${ZEPHYR_SAFETY_ISLAND_MODULE}/apps/actuation/boards/${BOARD}_actuation"
EXTRA_OECMAKE:append = "\
    -DDTC_OVERLAY_FILE='${OVERLAY_BASENAME_FILE}.overlay' \
    -DOVERLAY_CONFIG='${OVERLAY_BASENAME_FILE}.conf;${OVERLAY_ACTUATION_BASENAME_FILE}.conf' \
"

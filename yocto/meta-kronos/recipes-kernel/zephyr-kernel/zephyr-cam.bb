#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT
#

SUMMARY = "Critical Application Monitoring Service"
DESCRIPTION = "The Critical Application Monitoring (CAM) project implements a  \
               solution for monitoring applications using a service running on \
               a higher safety level system."
HOMEPAGE = "https://cam.docs.arm.com/"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://license.rst;md5=214c73aa30e7f71d6173261fe950f51b"

require recipes-kernel/zephyr-kernel/zephyr-sample.inc
require recipes-demos/cam/critical-application-monitoring-common.inc

EXTRA_OECMAKE:append = "\
    -DCAM_TARGET=Zephyr \
"
ZEPHYR_CAM_PATCHDIR = "modules/lib/critical-app-monitoring"
ZEPHYR_CAM_PATCHES ?= "\
    file://critical-app-monitoring/0001-Remove-Deprecated-CONFIG_DISK_RAM_VOLUME_SIZE-from-p.patch;patchdir=${ZEPHYR_CAM_PATCHDIR} \
"

SRC_URI:append = " ${SRC_URI_CAM};${BRANCH_CAM};name=critical-app-monitoring;destsuffix=git/modules/lib/critical-app-monitoring"
SRC_URI += "${ZEPHYR_CAM_PATCHES}"
SRCREV ?= "${SRCREV_CAM}"

ZEPHYR_SRC_DIR = "${S}/modules/lib/critical-app-monitoring"

OVERLAY_BASENAME_FILE = "${ZEPHYR_SAFETY_ISLAND_MODULE}/overlays/hipc/${ZEPHYR_BOARD}"
OVERLAY_CAM_BASENAME_FILE = "${ZEPHYR_SAFETY_ISLAND_MODULE}/apps/cam/boards/${ZEPHYR_BOARD}_cam"
OVERLAY_GPTP_BASENAME_FILE = "${ZEPHYR_SAFETY_ISLAND_MODULE}/overlays/gptp/${ZEPHYR_BOARD}"
EXTRA_OECMAKE:append = "\
    -DDTC_OVERLAY_FILE='${OVERLAY_BASENAME_FILE}.overlay;${OVERLAY_CAM_BASENAME_FILE}.overlay' \
    -DOVERLAY_CONFIG='${OVERLAY_BASENAME_FILE}.conf;${OVERLAY_CAM_BASENAME_FILE}.conf;${OVERLAY_GPTP_BASENAME_FILE}.conf' \
"

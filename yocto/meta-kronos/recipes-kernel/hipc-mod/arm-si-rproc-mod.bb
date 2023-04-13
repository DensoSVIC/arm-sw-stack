#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "ARM Safety Island remoteproc kernel module"
DESCRIPTION = "A driver for remote communications to the Safety Island core"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

inherit module

FILESEXTRAPATHS:prepend := "${KRONOS_REPO_DIRECTORY}/components/primary_compute/linux_drivers/arm_si_rproc_mod:"
SRC_URI = "file://src"
S = "${WORKDIR}/src"

RRECOMMENDS:${PN} += "kernel-module-arm-mhuv2"

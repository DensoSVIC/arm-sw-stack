#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "ARM Safety Island remoteproc kernel module"
DESCRIPTION = "A driver for remote communications to the Safety Island core"
HOMEPAGE = "https://arm-auto-solutions.docs.arm.com/"
LICENSE = "GPL-2.0-only"
# License file is in "layers/poky/meta/files/common-licenses".
# nooelint: oelint.var.licenseremotefile
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"
# FILESEXTRAPATHS is used to reference source code elsewhere in
# the same repository
# nooelint: oelint.vars.fileextrapaths
FILESEXTRAPATHS:prepend := "${ARM_AUTO_SOLUTIONS_REPO_DIRECTORY}/components/primary_compute/linux_drivers/arm_si_rproc_mod:"
SRC_URI = "file://src"
S = "${WORKDIR}/src"

inherit module

RRECOMMENDS:${PN} += "kernel-module-arm-mhuv3"

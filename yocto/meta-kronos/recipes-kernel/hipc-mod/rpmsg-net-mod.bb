#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "RPMSG network driver"
DESCRIPTION = "A network driver using RPMSG for its transport"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

inherit module

FILESEXTRAPATHS:prepend := "${KRONOS_REPO_DIRECTORY}/components/primary_compute/linux_drivers/rpmsg_net_mod:"
SRC_URI = "file://src"
S = "${WORKDIR}/src"

RRECOMMENDS:${PN} += "kernel-module-virtio-rpmsg-bus"

#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "Safety Island Hello World application"
DESCRIPTION = "A baremetal application which prints Hello World on a PL011 UART then enters a WFI loop"

LICENSE = "MIT"
# License file is in "layers/poky/meta/files/common-licenses".
# nooelint: oelint.var.licenseremotefile
LIC_FILES_CHKSUM ?= "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
SRC_URI = "file://src"
S = "${WORKDIR}/src"
COMPATIBLE_MACHINE = "fvp-rd-kronos"

inherit meson deploy

FILES:${PN} += "/firmware"
SYSROOT_DIRS += "/firmware"

do_deploy() {
    cp ${D}/firmware/${PN}.bin ${DEPLOYDIR}/${PN}.bin
}
addtask deploy after do_install

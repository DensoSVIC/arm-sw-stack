#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "Kronos network configuration"
DESCRIPTION = "Systemd configuration files for network interfaces \
for the Kronos stacks"
HOMEPAGE = "https://kronos.docs.arm.com/"
LICENSE = "MIT"
# License file is in "layers/poky/meta/files/common-licenses".
# nooelint: oelint.var.licenseremotefile
LIC_FILES_CHKSUM = "\
    file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302 \
    "

SRC_URI = "file://common/network;subdir=src"
SRC_URI:domu = "file://domu/network;subdir=src"
SRC_URI:append:virtualization = " file://virtualization/network;subdir=src"

S = "${WORKDIR}/src"

inherit allarch

do_configure[noexec] = "1"
do_compile[noexec] = "1"

NETWORK_CONF_DIR = "${sysconfdir}/systemd/network"

do_install() {
    install -d ${D}${NETWORK_CONF_DIR}
    install -D ${S}/*/network/* ${D}${NETWORK_CONF_DIR}
}

FILES:${PN} += "${NETWORK_CONF_DIR}"

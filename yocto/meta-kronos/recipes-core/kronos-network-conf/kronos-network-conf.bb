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
LIC_FILES_CHKSUM = "\
    file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302 \
    "

SRC_URI:baremetal = "file://baremetal/network"
SRC_URI:virtualization = "file://virtualization/network"
SRC_URI:domu = "file://domu/network"

inherit allarch

do_configure[noexec] = "1"
do_compile[noexec] = "1"

NETWORK_CONF_DIR = "${sysconfdir}/systemd/network"

do_install() {
    install -d ${D}${NETWORK_CONF_DIR}
    install -D ${WORKDIR}/*/network/* ${D}${NETWORK_CONF_DIR}
}

FILES:${PN} += "${NETWORK_CONF_DIR}"

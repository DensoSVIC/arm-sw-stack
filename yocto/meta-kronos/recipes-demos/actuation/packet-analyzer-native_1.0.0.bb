#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "Packet Analyzer"
DESCRIPTION = "The Packet Analyzer validates the Control Commands emitted by the Actuation Service running within the SI."
HOMEPAGE = "https://safety-island-actuation-demo.docs.arm.com/"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://license.rst;md5=e805dc5353977631b7881c7705a6c04a"

require actuation-comon.inc

PV .= "+git${SRCPV}"
SRC_URI = "${SRC_URI_ACTUATION};${BRANCH_ACTUATION}"
SRCREV = "${SRCREV_ACTUATION}"
S = "${WORKDIR}/git"

inherit python3native native

RDEPENDS:${PN} += "python3-numpy-native"

do_configure[noexec] = "1"
do_compile[noexec] = "1"
do_install() {
    install -d ${D}/${bindir}/actuation_packet_analyzer
    cp -r ${S}/packet_analyzer ${D}/${bindir}/actuation_packet_analyzer
}

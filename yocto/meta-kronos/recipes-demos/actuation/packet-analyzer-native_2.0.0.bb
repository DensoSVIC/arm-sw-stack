#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "Packet Analyzer"
DESCRIPTION = "The Packet Analyzer validates the Control Commands emitted by the Actuation Service running within the SI."
HOMEPAGE = "https://safety-island-actuation-demo.docs.arm.com/"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://license.rst;md5=e805dc5353977631b7881c7705a6c04a"

require actuation-common.inc

DEPENDS += "python3-setuptools-scm-native"

PV .= "+git${SRCPV}"
SRC_URI = "${SRC_URI_ACTUATION};${BRANCH_ACTUATION}"
SRCREV = "${SRCREV_ACTUATION}"
S = "${WORKDIR}/git"

inherit python_setuptools_build_meta native

PEP517_SOURCE_PATH = "${S}/packet_analyzer"
RDEPENDS:${PN} += "python3-numpy-native"

# Using addtask to reorder the dependency of addto_recipe_sysroot
# nooelint: oelint.task.addnotaskbody
addtask addto_recipe_sysroot after do_populate_sysroot before do_build

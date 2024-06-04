#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "Actuation Player"
DESCRIPTION = "The Actuation Player plays back a recorded trajectory \
for the Actuation Service to process."
HOMEPAGE = "https://safety-island-actuation-demo.docs.arm.com/"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://license.rst;md5=e805dc5353977631b7881c7705a6c04a"

require actuation-pkgs.inc

# DEPENDS gets its values from actuation-pkgs.inc, hence ignoring ordered list
# check
# nooelint: oelint.vars.dependsordered
DEPENDS += "actuation-msgs cyclonedds"

OECMAKE_SOURCEPATH = "${S}/actuation_packages/actuation_player"

FILES:${PN} += "${datadir}/* ${libdir}/actuation_player/*"

RDEPENDS:${PN} += "\
    actuation-msgs \
    cyclonedds \
"

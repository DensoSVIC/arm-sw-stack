#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "Actuation Player"
DESCRIPTION = "The Actuation Player plays back a recorded trajectory \
for the Actuation Service to process."
HOMEPAGE = "https://safety-island-actuation-demo.docs.arm.com/"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://license.rst;md5=e805dc5353977631b7881c7705a6c04a"

require actuation-comon.inc

DEPENDS += "\
    actuation-msgs \
    ament-cmake-auto-native \
    ament-cmake-python-native \
    ament-cmake-target-dependencies-native \
    autoware-cmake-native \
    cyclonedds \
"

PV .= "+git${SRCPV}"
SRC_URI = "${SRC_URI_ACTUATION};${BRANCH_ACTUATION}"
SRCREV = "${SRCREV_ACTUATION}"
S = "${WORKDIR}/git"

inherit python3native pkgconfig cmake

FILES:${PN} += "${datadir}/* ${libdir}/actuation_player/*"

RDEPENDS:${PN} += "\
    actuation-msgs \
    cyclonedds \
"

OECMAKE_SOURCEPATH = "${S}/actuation_packages/actuation_player"

EXTRA_OECMAKE:append = "\
    -DBUILD_TESTING=OFF \
    -DROS_DISTRO=galactic \
"

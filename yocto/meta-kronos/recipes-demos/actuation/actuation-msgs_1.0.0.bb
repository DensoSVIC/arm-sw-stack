#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "Actuation Msgs"
DESCRIPTION = "IDLC messages for the Actuation Service."
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://license.rst;md5=e805dc5353977631b7881c7705a6c04a"

require actuation-comon.inc

inherit python3native pkgconfig cmake

DEPENDS += "\
    ament-cmake-auto-native \
    ament-cmake-python-native \
    ament-cmake-target-dependencies-native \
    autoware-cmake-native \
    cyclonedds-native \
"

SRC_URI = "${SRC_URI_ACTUATION};${BRANCH_ACTUATION}"
SRCREV = "${SRCREV_ACTUATION}"
S = "${WORKDIR}/git"

OECMAKE_SOURCEPATH = "${S}/actuation_packages/actuation_msgs"

EXTRA_OECMAKE:append = "\
    -DBUILD_SHARED_LIBS=ON \
    -DBUILD_TESTING=OFF \
    -DROS_DISTRO=galactic \
"

FILES:${PN} += "${datadir}/*"

PV .= "+git${SRCPV}"

BBCLASSEXTEND = "native nativesdk"

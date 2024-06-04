#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "Actuation Msgs"
DESCRIPTION = "IDLC messages for the Actuation Service."
HOMEPAGE = "https://safety-island-actuation-demo.docs.arm.com/"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://license.rst;md5=e805dc5353977631b7881c7705a6c04a"

require actuation-pkgs.inc

DEPENDS += "cyclonedds-native"

OECMAKE_SOURCEPATH = "${S}/actuation_packages/actuation_msgs"

EXTRA_OECMAKE:append = "\
    -DBUILD_SHARED_LIBS=ON \
"

FILES:${PN} += "${datadir}/*"

BBCLASSEXTEND = "native nativesdk"

#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "CMake scripts for Autoware"
DESCRIPTION = "This package provides CMake scripts for Autoware."
HOMEPAGE = "https://github.com/autowarefoundation"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://LICENSE;md5=86d3f3a95c324c9479bd8986968f4327"

DEPENDS += "\
    ament-cmake-auto-native \
    ament-cmake-python-native \
    ament-cmake-target-dependencies-native \
"

PV .= "+git${SRCPV}"
SRC_URI = "git://github.com/autowarefoundation/autoware_common.git;${BRANCH_AUTOWARE};protocol=https"
SRCREV = "44dc3880361f8e5b8eb4daf05119bc9446ff8303"
S = "${WORKDIR}/git"

inherit python3native pkgconfig cmake

BRANCH_AUTOWARE ?= "branch=main"

OECMAKE_SOURCEPATH = "${S}/autoware_cmake"

EXTRA_OECMAKE:append = "\
    -DBUILD_TESTING=OFF \
"

BBCLASSEXTEND = "native nativesdk"

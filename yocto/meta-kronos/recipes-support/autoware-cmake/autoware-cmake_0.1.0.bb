#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "CMake scripts for Autoware"
DESCRIPTION = "This package provides CMake scripts for Autoware."
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://LICENSE;md5=86d3f3a95c324c9479bd8986968f4327"

inherit python3native pkgconfig cmake

DEPENDS += "\
    ament-cmake-auto-native \
    ament-cmake-python-native \
    ament-cmake-target-dependencies-native \
"

BRANCH_AUTOWARE ?= "branch=galactic"
SRC_URI = "git://github.com/autowarefoundation/autoware_common.git;${BRANCH_AUTOWARE};protocol=https"
SRCREV = "0f5c64c7497462ac4e669a8bfb7ef1bd058a588d"
S = "${WORKDIR}/git"

OECMAKE_SOURCEPATH = "${S}/autoware_cmake"

EXTRA_OECMAKE:append = "\
    -DBUILD_TESTING=OFF \
"

PV .= "+git${SRCPV}"

BBCLASSEXTEND = "native nativesdk"

#
# Based on: https://github.com/ros/meta-ros/blob/875464b52828c6a0d6669f7009607e3ea28bc1a6/meta-ros2-humble/generated-recipes/cyclonedds/cyclonedds_0.10.3-1.bb
# In open-source project: meta-ros
# Original file: SPDX-FileCopyrightText: <text>Copyright 2023 Open Source
# Robotics Foundation</text>
# Modifications: SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited
# and/or its affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# Changes:
# 1) Remove ROS and iceoryx dependencies
# 2) Change target version
# 3) Add extra cmake arguments
# 4) Fix oelint-adv issues
#
SUMMARY = "Cyclone DDS"
DESCRIPTION = "Eclipse Cyclone DDS is a very performant and robust open-source \
DDS implementation. Cyclone DDS is developed completely in the open as an \
Eclipse IoT project."
AUTHOR = "Eclipse Foundation, Inc. <cyclonedds-dev@eclipse.org>"
HOMEPAGE = "https://projects.eclipse.org/projects/iot.cyclonedds"
SECTION = "devel"
# Original license in package.xml, joined with "&" when multiple license tags were used:
#         "Eclipse Public License 2.0 & Eclipse Distribution License 1.0"
LICENSE = "EPL-2.0 & EDL-1.0"
LIC_FILES_CHKSUM = "file://package.xml;beginline=8;endline=8;md5=7532470dee289492e850d7d3e8a32b32"

DEPENDS = "bison-native cyclonedds-native"

require cyclonedds_0.10.3.inc
PV .= "+git${SRCPV}"

SRC_URI = "${SRC_URI_CYCLONEDDS}"
SRCREV = "${SRCREV_CYCLONEDDS}"
S = "${WORKDIR}/git"

inherit pkgconfig cmake

EXTRA_OECMAKE:append = "\
    -DBUILD_EXAMPLES=OFF \
    -DENABLE_SECURITY=OFF \
    -DENABLE_SSL=OFF \
    -DBUILD_SHARED_LIBS=ON \
    -DENABLE_SHM=OFF \
    -DBUILD_TESTING=OFF \
    -DBUILD_DDSPERF=OFF \
"
EXTRA_OECMAKE:append:class-native = "\
    -DBUILD_IDLC=ON \
"
EXTRA_OECMAKE:append:class-target = "\
    -DBUILD_IDLC=OFF \
"

BBCLASSEXTEND = "native nativesdk"

#
# Based on: https://github.com/ros/meta-ros/blob/bfb09373ad8648c3c8d9f34ad13326d354a7d1ab/meta-ros2-galactic/generated-recipes/cyclonedds/cyclonedds_0.8.0-5.bb
# In open-source project: meta-ros
# Original file: SPDX-FileCopyrightText: <text>Copyright 2021 Open Source
# Robotics Foundation</text>
# Modifications: SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited
# and/or its affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# Changes:
# 1) Remove ROS and iceoryx dependencies
# 2) Change target version
# 3) Add extra cmake arguments
#
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

inherit pkgconfig cmake

SRC_URI = "${SRC_URI_CYCLONEDDS}"
SRCREV = "${SRCREV_CYCLONEDDS}"
S = "${WORKDIR}/git"

EXTRA_OECMAKE:append = "\
    -DBUILD_EXAMPLES=OFF \
    -DENABLE_SECURITY=OFF \
    -DENABLE_SSL=OFF \
    -DBUILD_SHARED_LIBS=ON \
    -DENABLE_SHM=OFF \
    -DBUILD_TESTING=OFF \
    -DBUILD_DDSPERF=OFF \
"
EXTRA_OECMAKE:class-native:append = "\
    -DBUILD_IDLC=ON \
"
EXTRA_OECMAKE:class-target:append = "\
    -DBUILD_IDLC=OFF \
"

BBCLASSEXTEND = "native nativesdk"

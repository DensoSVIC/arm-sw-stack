#
# Based on: https://github.com/ros/meta-ros/blob/746cd438c81a858d486a9008b1cecf80831cbddf/meta-ros2-galactic/generated-recipes/ament-cmake/ament-cmake-version_1.1.4-1.bb
# In open-source project: meta-ros
# Original file: SPDX-FileCopyrightText: <text>Copyright 2021 Open Source
# Robotics Foundation</text>
# Modifications: SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited
# and/or its affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# Changes:
# 1) Remove ROS dependencies
# 2) Add BBCLASSEXTEND
#
DESCRIPTION = "The ability to override the exported package version in the ament buildsystem."
AUTHOR = "Dirk Thomas <dthomas@osrfoundation.org>"
HOMEPAGE = "https://wiki.ros.org"
SECTION = "devel"
# Original license in package.xml, joined with "&" when multiple license tags were used:
#         "Apache License 2.0"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://package.xml;beginline=9;endline=9;md5=12c26a18c7f493fdc7e8a93b16b7c04f"

ROS_CN = "ament_cmake"
ROS_BPN = "ament_cmake_version"

ROS_BUILD_DEPENDS = ""

ROS_BUILDTOOL_DEPENDS = " \
    ament-cmake-core-native \
"

ROS_EXPORT_DEPENDS = ""

ROS_BUILDTOOL_EXPORT_DEPENDS = " \
    ament-cmake-core-native \
"

ROS_EXEC_DEPENDS = ""

# Currently informational only -- see http://www.ros.org/reps/rep-0149.html#dependency-tags.
ROS_TEST_DEPENDS = ""

DEPENDS = "${ROS_BUILD_DEPENDS} ${ROS_BUILDTOOL_DEPENDS}"
# Bitbake doesn't support the "export" concept, so build them as if we needed them to build this package (even though we actually
# don't) so that they're guaranteed to have been staged should this package appear in another's DEPENDS.
DEPENDS += "${ROS_EXPORT_DEPENDS} ${ROS_BUILDTOOL_EXPORT_DEPENDS}"

RDEPENDS:${PN} += "${ROS_EXEC_DEPENDS}"

# matches with: https://github.com/ros2-gbp/ament_cmake-release/archive/release/galactic/ament_cmake_version/1.1.4-1.tar.gz
ROS_BRANCH ?= "branch=release/galactic/ament_cmake_version"
SRC_URI = "git://github.com/ros2-gbp/ament_cmake-release;${ROS_BRANCH};protocol=https"
SRCREV = "f983d75326fdb11add959adb3ff5b435555e0893"
S = "${WORKDIR}/git"

ROS_BUILD_TYPE = "ament_cmake"

inherit ros_${ROS_BUILD_TYPE}

BBCLASSEXTEND = "native nativesdk"

#
# Based on: https://github.com/ros/meta-ros/blob/875464b52828c6a0d6669f7009607e3ea28bc1a6/meta-ros2-humble/generated-recipes/ament-cmake/googletest/gtest-vendor_1.10.9004-4.bb
#
# In open-source project: meta-ros
# Original file: SPDX-FileCopyrightText: <text>Copyright 2023 Open Source
# Robotics Foundation</text>
# Modifications: SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited
# and/or its affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# Changes:
# 1) Remove ROS dependencies
# 2) Add BBCLASSEXTEND
# 3) Rename BSD license to BSD-3-Clause
#
SUMMARY = "Google Test"
DESCRIPTION = "The package provides GoogleTest."
AUTHOR = "Audrow Nash <audrow@openrobotics.org>"
ROS_AUTHOR = "Dirk Thomas <dthomas@osrfoundation.org>"
HOMEPAGE = "https://wiki.ros.org"
SECTION = "devel"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://package.xml;beginline=40;endline=40;md5=d566ef916e9dedc494f5f793a6690ba5"

ROS_CN = "googletest"
ROS_BPN = "gtest_vendor"

ROS_BUILD_DEPENDS = "\
    cmake \
"

ROS_BUILDTOOL_DEPENDS = ""

ROS_EXPORT_DEPENDS = ""

ROS_BUILDTOOL_EXPORT_DEPENDS = ""

ROS_EXEC_DEPENDS = ""

# Currently informational only -- see http://www.ros.org/reps/rep-0149.html#dependency-tags.
ROS_TEST_DEPENDS = ""

DEPENDS = "${ROS_BUILD_DEPENDS} ${ROS_BUILDTOOL_DEPENDS}"
# Bitbake doesn't support the "export" concept, so build them as if we needed them to build this package (even though we actually
# don't) so that they're guaranteed to have been staged should this package appear in another's DEPENDS.
DEPENDS += "${ROS_BUILDTOOL_EXPORT_DEPENDS} ${ROS_EXPORT_DEPENDS}"

# matches with: https://github.com/ros2-gbp/googletest-release/archive/release/humble/gtest_vendor/1.10.9004-4.tar.gz
ROS_BRANCH ?= "branch=release/humble/gtest_vendor"
SRC_URI = "git://github.com/ros2-gbp/googletest-release;${ROS_BRANCH};protocol=https"
SRCREV = "20131d7c09d8ddecdce7342c7bc482da0dabca48"
S = "${WORKDIR}/git"

ROS_BUILD_TYPE = "ament_cmake"

inherit ros_${ROS_BUILD_TYPE}

RDEPENDS:${PN} += "${ROS_EXEC_DEPENDS}"

BBCLASSEXTEND = "native nativesdk"

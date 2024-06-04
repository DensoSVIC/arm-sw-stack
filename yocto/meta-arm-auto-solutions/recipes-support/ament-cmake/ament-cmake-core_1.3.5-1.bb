#
# Based on: https://github.com/ros/meta-ros/blob/875464b52828c6a0d6669f7009607e3ea28bc1a6/meta-ros2-humble/generated-recipes/ament-cmake/ament-cmake-core_1.3.5-1.bb
# In open-source project: meta-ros
# Original file: SPDX-FileCopyrightText: <text>Copyright 2021 Open Source
# Robotics Foundation</text>
# Modifications: SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited
# and/or its affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# Changes:
# 1) Remove ROS dependencies
# 2) Add BBCLASSEXTEND
# 3) Fix oelint-adv issues
#
SUMMARY = "Ament CMake Core"
DESCRIPTION = "The core of the ament buildsystem in CMake.      Several subcomponents provide specific funtionalities:     * environment: provide prefix-level setup files     * environment_hooks: provide package-level setup files and environment hooks     * index: store information in an index and retrieve them without crawling     * package_templates: templates from the ament_package Python package     * symlink_install: use symlinks for CMake install commands"
AUTHOR = "Michael Jeronimo <michael.jeronimo@openrobotics.org>"
ROS_AUTHOR = "Dirk Thomas"
HOMEPAGE = "https://wiki.ros.org"
SECTION = "devel"
# Original license in package.xml, joined with "&" when multiple license tags were used:
#         "Apache License 2.0"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://package.xml;beginline=20;endline=20;md5=12c26a18c7f493fdc7e8a93b16b7c04f"

ROS_CN = "ament_cmake"
ROS_BPN = "ament_cmake_core"

ROS_BUILD_DEPENDS = ""

ROS_BUILDTOOL_DEPENDS = "\
    ament-package-native \
    cmake-native \
    python3-catkin-pkg-native \
"

ROS_EXPORT_DEPENDS = ""

ROS_BUILDTOOL_EXPORT_DEPENDS = "\
    ament-package-native \
    cmake-native \
    python3-catkin-pkg-native \
"

ROS_EXEC_DEPENDS = ""

# Currently informational only -- see http://www.ros.org/reps/rep-0149.html#dependency-tags.
ROS_TEST_DEPENDS = ""

DEPENDS = "${ROS_BUILD_DEPENDS} ${ROS_BUILDTOOL_DEPENDS}"
# Bitbake doesn't support the "export" concept, so build them as if we needed them to build this package (even though we actually
# don't) so that they're guaranteed to have been staged should this package appear in another's DEPENDS.
DEPENDS += "${ROS_BUILDTOOL_EXPORT_DEPENDS} ${ROS_EXPORT_DEPENDS}"

# matches with: https://github.com/ros2-gbp/ament_cmake-release/archive/release/humble/ament_cmake_core/1.3.5-1.tar.gz
ROS_BRANCH ?= "branch=release/humble/ament_cmake_core"
SRC_URI = "git://github.com/ros2-gbp/ament_cmake-release;${ROS_BRANCH};protocol=https"
SRCREV = "2279ac31dd2794e585f56c66bf86b21568b0fd7f"
S = "${WORKDIR}/git"

ROS_BUILD_TYPE = "ament_cmake"

inherit ros_${ROS_BUILD_TYPE}

RDEPENDS:${PN} += "${ROS_EXEC_DEPENDS}"

BBCLASSEXTEND = "native nativesdk"

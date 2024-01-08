#
# Based on: https://github.com/ros/meta-ros/blob/875464b52828c6a0d6669f7009607e3ea28bc1a6/meta-ros2-humble/generated-recipes/ament-package/ament-package_0.14.0-4.bb
# In open-source project: meta-ros
# Original file: SPDX-FileCopyrightText: <text>Copyright 2023 Open Source
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
SUMMARY = "Ament Parser"
DESCRIPTION = "The parser for the manifest files in the ament buildsystem."
AUTHOR = "Mabel Zhang <mabel@openrobotics.org>"
ROS_AUTHOR = "Dirk Thomas <dthomas@osrfoundation.org>"
HOMEPAGE = "https://wiki.ros.org"
SECTION = "devel"
# Original license in package.xml, joined with "&" when multiple license tags were used:
#         "Apache License 2.0"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://package.xml;beginline=10;endline=10;md5=12c26a18c7f493fdc7e8a93b16b7c04f"

ROS_CN = "ament_package"
ROS_BPN = "ament_package"

ROS_BUILD_DEPENDS = "\
    python3 \
    python3-importlib-metadata \
    python3-setuptools \
"

ROS_BUILDTOOL_DEPENDS = ""

ROS_EXPORT_DEPENDS = "\
    python3 \
    python3-importlib-metadata \
    python3-setuptools \
"

ROS_BUILDTOOL_EXPORT_DEPENDS = ""

ROS_EXEC_DEPENDS = "\
    python3 \
    python3-importlib-metadata \
    python3-setuptools \
"

# Currently informational only -- see http://www.ros.org/reps/rep-0149.html#dependency-tags.
ROS_TEST_DEPENDS = "\
    python3-flake8 \
    python3-pytest \
"

DEPENDS = "${ROS_BUILD_DEPENDS} ${ROS_BUILDTOOL_DEPENDS}"
# Bitbake doesn't support the "export" concept, so build them as if we needed them to build this package (even though we actually
# don't) so that they're guaranteed to have been staged should this package appear in another's DEPENDS.
DEPENDS += "${ROS_BUILDTOOL_EXPORT_DEPENDS} ${ROS_EXPORT_DEPENDS}"

# matches with: https://github.com/ros2-gbp/ament_package-release/archive/release/humble/ament_package/0.14.0-4.tar.gz
ROS_BRANCH ?= "branch=release/humble/ament_package"
SRC_URI = "git://github.com/ros2-gbp/ament_package-release;${ROS_BRANCH};protocol=https"
SRCREV = "00da147b17c19bc225408dc693ed8fdc14c314ab"
S = "${WORKDIR}/git"

ROS_BUILD_TYPE = "ament_python"

inherit ros_${ROS_BUILD_TYPE}

RDEPENDS:${PN} += "${ROS_EXEC_DEPENDS}"

BBCLASSEXTEND = "native nativesdk"

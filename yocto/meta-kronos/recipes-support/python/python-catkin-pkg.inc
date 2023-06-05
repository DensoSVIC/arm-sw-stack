#
# Based on: https://github.com/ros/meta-ros/blob/bfb09373ad8648c3c8d9f34ad13326d354a7d1ab/meta-ros-common/recipes-infrastructure/python/python-catkin-pkg.inc
# In open-source project: meta-ros
# Original file: SPDX-FileCopyrightText: <text>Copyright 2020-2021 meta-ros
# community</text>
# Modifications: SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited
# and/or its affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# Changes:
# 1) Update target version
# 2) Fix oelint-adv issues
#
DESCRIPTION = "catkin package library"
SECTION = "devel/python"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://PKG-INFO;beginline=8;endline=8;md5=e910b35b0ef4e1f665b9a75d6afb7709"
PYPI_PACKAGE = "catkin_pkg"

DEPENDS += "${PYTHON_PN}-pyparsing"

SRC_URI[sha256sum] = "5d643eeafbce4890fcceaf9db197eadf2ca5a187d25593f65b6e5c57935f5da2"

inherit pypi

RDEPENDS:${PN} = "${PYTHON_PN}-dateutil ${PYTHON_PN}-docutils ${PYTHON_PN}-pyparsing ${PYTHON_PN}-unixadmin"
RDEPENDS:${PN}:class-native = ""

BBCLASSEXTEND = "native nativesdk"

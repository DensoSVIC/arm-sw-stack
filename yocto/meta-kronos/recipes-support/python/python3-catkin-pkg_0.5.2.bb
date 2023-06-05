#
# Based on: https://github.com/ros/meta-ros/blob/bfb09373ad8648c3c8d9f34ad13326d354a7d1ab/meta-ros-common/recipes-infrastructure/python/python3-catkin-pkg_0.4.24.bb
# In open-source project: meta-ros
# Original file: SPDX-FileCopyrightText: <text>Copyright 2021 meta-ros
# community</text>
# Modifications: SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited
# and/or its affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# Changes:
# 1) Update target version
# 2) Fix oelint-adv issues
#
SUMMARY = "Python3 Catkin Package"
HOMEPAGE = "http://wiki.ros.org/catkin_pkg"

require python-catkin-pkg.inc

inherit setuptools3

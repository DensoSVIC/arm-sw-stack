#
# Based on: https://github.com/ros/meta-ros/blob/875464b52828c6a0d6669f7009607e3ea28bc1a6/meta-ros-common/recipes-infrastructure/python/python3-catkin-pkg_0.4.24.bb
# In open-source project: meta-ros
# Original file: SPDX-FileCopyrightText: <text>Copyright 2021 meta-ros
# community</text>
# Modifications: SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited
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

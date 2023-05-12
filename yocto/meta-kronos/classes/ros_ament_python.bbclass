#
# Based on: https://github.com/ros/meta-ros/blob/746cd438c81a858d486a9008b1cecf80831cbddf/meta-ros2/classes/ros_ament_cmake.bbclass
# In open-source project: meta-ros
# Original file: SPDX-FileCopyrightText: <text>Copyright 2018-2021
# LG Electronics, Inc., meta-ros community</text>
# Modifications: SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited
# and/or its affiliates <open-source-office@arm.com></text>
# SPDX-License-Identifier: MIT

inherit setuptools3

do_install:append() {
    mkdir -p ${D}${datadir}/ament_index/resource_index/packages
    touch ${D}${datadir}/ament_index/resource_index/packages/${ROS_BPN}
    if test -e ${D}${libdir}/${ROS_BPN}; then
        for i in ${D}${libdir}/${ROS_BPN}/* ; do
            sed -i '1c#!/usr/bin/python3' $i
        done
    fi
}

FILES:${PN}:prepend = " \
    ${datadir}/ament_index \
"

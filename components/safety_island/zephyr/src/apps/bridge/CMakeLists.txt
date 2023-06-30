# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23.0)

set(CONF_FILE "prj.conf")
# Do not overwrite the OVERLAY_CONFIG value provided by the user.
set(OVERLAY_CONFIG "${OVERLAY_CONFIG}" CACHE STRING "overlay config" FORCE)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(zephyr_bridge)

target_sources(app PRIVATE
  src/main.c
)

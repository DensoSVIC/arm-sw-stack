# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(fault_mgmt)

target_sources(app PRIVATE
  src/main.c
)

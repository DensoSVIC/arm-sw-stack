#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

require recipes-bsp/u-boot/u-boot-common.inc
require recipes-bsp/u-boot/u-boot.inc

SRC_URI = "git://source.denx.de/u-boot/u-boot.git;protocol=https;branch=u-boot-2023.07.y"
SRCREV = "83cdab8b2c6ea0fc0860f8444d083353b47f1d5c"

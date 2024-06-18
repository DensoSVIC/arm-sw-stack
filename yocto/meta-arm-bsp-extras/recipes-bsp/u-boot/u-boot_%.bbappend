#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

MACHINE_EXTRAS_U-BOOT_REQUIRE ?= ""
MACHINE_EXTRAS_U-BOOT_REQUIRE:fvp-rd-kronos = "u-boot-fvp-rd-kronos.inc"

require ${MACHINE_EXTRAS_U-BOOT_REQUIRE}

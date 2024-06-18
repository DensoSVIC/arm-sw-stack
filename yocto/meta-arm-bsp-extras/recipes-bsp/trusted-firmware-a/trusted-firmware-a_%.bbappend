#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend := "${THISDIR}/files/:"

MACHINE_EXTRAS_TFA_REQUIRE ?= ""
MACHINE_EXTRAS_TFA_REQUIRE:fvp-rd-kronos = "trusted-firmware-a-fvp-rd-kronos.inc"

require ${MACHINE_EXTRAS_TFA_REQUIRE}

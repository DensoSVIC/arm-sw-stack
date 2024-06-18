#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

LINUX_ARM_BSP_EXTRAS_REQUIRE ?= ""
LINUX_ARM_BSP_EXTRAS_REQUIRE:fvp-rd-kronos = "linux-yocto-fvp-rd-kronos.inc"

require ${LINUX_ARM_BSP_EXTRAS_REQUIRE}

#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# Include machine specific Linux Yocto configurations

MACHINE_LINUX_YOCTO_REQUIRE ?= ""
MACHINE_LINUX_YOCTO_REQUIRE:fvp-rd-kronos = "linux-yocto-fvp-rd-kronos.inc"

require ${MACHINE_LINUX_YOCTO_REQUIRE}

#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# Include machine specific Linux Yocto configurations

MACHINE_LINUX_YOCTO_REQUIRE ?= ""
MACHINE_LINUX_YOCTO_REQUIRE:baremetal = "linux-yocto-arm-auto-solutions.inc"
MACHINE_LINUX_YOCTO_REQUIRE:virtualization = "linux-yocto-arm-auto-solutions.inc"

require ${MACHINE_LINUX_YOCTO_REQUIRE}

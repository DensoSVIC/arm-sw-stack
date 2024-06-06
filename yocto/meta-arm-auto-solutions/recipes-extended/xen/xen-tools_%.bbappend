#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

MACHINE_XEN_TOOLS_REQUIRE ?= ""
MACHINE_XEN_TOOLS_REQUIRE:virtualization = "\
    xen-arm-auto-solutions.inc \
    xen-tools-arm-auto-solutions.inc \
    "
require ${MACHINE_XEN_TOOLS_REQUIRE}

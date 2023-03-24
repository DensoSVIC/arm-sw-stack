#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT
#

MACHINE_INIT_IFUPDOWN_REQUIRE ?= ""
MACHINE_INIT_IFUPDOWN_REQUIRE:fvp-rd-kronos = \
    "init-ifupdown-extras.inc"
MACHINE_INIT_IFUPDOWN_REQUIRE:generic-arm64 = \
    "init-ifupdown-extras.inc"

require ${MACHINE_INIT_IFUPDOWN_REQUIRE}

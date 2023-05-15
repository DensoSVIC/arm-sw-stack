#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

MACHINE_LIBEIGEN_REQUIRE ?= ""
MACHINE_LIBEIGEN_REQUIRE:fvp-rd-kronos = \
    "libeigen_3.3.7.inc"
MACHINE_LIBEIGEN_REQUIRE:generic-arm64 = \
    "libeigen_3.3.7.inc"

require ${MACHINE_LIBEIGEN_REQUIRE}

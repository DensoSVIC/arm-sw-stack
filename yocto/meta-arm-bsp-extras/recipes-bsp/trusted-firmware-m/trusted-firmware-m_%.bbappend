#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# Machine specific configurations

MACHINE_TFM_REQUIRE ?= ""
MACHINE_TFM_REQUIRE:fvp-rd-kronos = "trusted-firmware-m-fvp-rd-kronos.inc"

require ${MACHINE_TFM_REQUIRE}

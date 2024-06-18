#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# Include machine specific SCP configurations

MACHINE_SCP_REQUIRE ?= ""
MACHINE_SCP_REQUIRE:fvp-rd-kronos = "scp-firmware-fvp-rd-kronos.inc"

require ${MACHINE_SCP_REQUIRE}

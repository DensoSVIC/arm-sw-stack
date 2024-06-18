#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# Include Trusted Services Secure Partitions
require recipes-security/optee/optee-os-ts.inc

# Machine specific configurations
MACHINE_EXTRAS_OPTEE_OS_REQUIRE ?= ""
MACHINE_EXTRAS_OPTEE_OS_REQUIRE:fvp-rd-kronos = "optee-os-fvp-rd-kronos.inc"

require ${MACHINE_EXTRAS_OPTEE_OS_REQUIRE}

#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

COMPATIBLE_MACHINE:fvp-rd-kronos = "fvp-rd-kronos"

# The ts-tests-psa relies on the ts-se-proxy MACHINE_FEATURES, which is enabled
# on the fvp-rd-kronos machine in its machine configuration.
RDEPENDS:${PN}-psa:fvp-rd-kronos = "ts-psa-crypto-api-test ts-psa-ps-api-test"

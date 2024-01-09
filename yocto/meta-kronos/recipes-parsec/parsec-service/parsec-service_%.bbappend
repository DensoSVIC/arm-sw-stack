#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

# Set the provider as Trusted service only for baremetal
PACKAGECONFIG:fvp-rd-kronos = "${@bb.utils.contains('IMAGE_FEATURES', 'baremetal', \
                               'TS', 'MBED-CRYPTO', d)}"

PACKAGECONFIG:generic-arm64 = "MBED-CRYPTO"

#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

# Reuse the patch in cassini to setup the provider as Trusted service in config.toml
SRC_URI:append:fvp-rd-kronos := " ${@bb.utils.contains('IMAGE_FEATURES', 'baremetal', \
                               'file://0001-cassini-bsp-Enable-parse-service-to-use-TS.patch', '', d)}"

# Set the provider as Trusted service only for baremetal
PACKAGECONFIG:fvp-rd-kronos = "${@bb.utils.contains('IMAGE_FEATURES', 'baremetal', \
                               'TS', 'MBED-CRYPTO', d)}"

PACKAGECONFIG:generic-arm64 = "MBED-CRYPTO"

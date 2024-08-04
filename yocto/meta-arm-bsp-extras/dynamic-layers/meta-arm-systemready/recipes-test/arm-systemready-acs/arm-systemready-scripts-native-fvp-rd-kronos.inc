#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend := "${THISDIR}/files/${MACHINE}:"

# python3-dtschema-wrapper-native is to allow kernel dt-validation to pass
# real function is not implemented and always return succeed
RDEPENDS:${PN}:remove = "python3-dtschema-wrapper-native"

# need python3-dtschema for dt-validate
RDEPENDS:${PN}:append = " python3-dtschema-native"

# cSpell: disable
SRC_URI:append = " \
    file://0003-check-sr-results-Change-the-expected-SR-result-confi.patch \
    file://0004-check-sr-results-Change-extra-compat.patch \
    file://0005-check-sr-results.yaml-Reset-SCT-failure-number-to-0.patch \
    file://0006-dt-parser-Change-rule-for-root-cpu-and-cache-control.patch \
"
# cSpell: enable

PV = "ir2.1-${SRCPV}"
# The SRCREV is in the branch ir2.1
SRCREV  = "d8eab81d2d8564cbe2fd648d9dff5d9c1eaadb4c"

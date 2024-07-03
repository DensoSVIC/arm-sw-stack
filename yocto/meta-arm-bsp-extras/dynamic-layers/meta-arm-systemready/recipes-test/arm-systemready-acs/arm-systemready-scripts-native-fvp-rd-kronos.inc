#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend := "${THISDIR}/files/${MACHINE}:"

# cSpell: disable
SRC_URI:append = " \
    file://0003-check-sr-results-Change-the-expected-SR-result-confi.patch \
"
# cSpell: enable

PV = "ir2.1-${SRCPV}"
# The SRCREV is in the branch ir2.1
SRCREV  = "d8eab81d2d8564cbe2fd648d9dff5d9c1eaadb4c"

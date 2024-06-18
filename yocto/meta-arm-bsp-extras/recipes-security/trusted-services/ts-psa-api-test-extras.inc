#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend:fvp-rd-kronos := "${THISDIR}/files/fvp-rd-kronos/psa-apitest:"

include ts-arm-platforms-extras.inc

SRCREV_psatest:fvp-rd-kronos = "334e110528f975186e601fb35b6250e770635467"
PV:fvp-rd-kronos = "v23.06_API1.5_ADAC_EAC"

# The fvp-rd-kronos will reuse corstone1000 patches
SRC_URI:append:fvp-rd-kronos = " \
    file://0001-corstone1000-port-crypto-config.patch;patchdir=../psatest \
    file://0002-corstone1000-Disable-obsolete-algorithms.patch;patchdir=../psatest \
    file://0003-corstone1000-Disable-SHA512-384.patch;patchdir=../psatest \
    file://0004-kronos-Disable-failing-crypto-tests.patch;patchdir=../psatest \
    "

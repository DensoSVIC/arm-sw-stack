#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend := "${THISDIR}:"
SRC_URI_EXTRAS_KMETA = "file://arm-platforms-extra-kmeta;type=kmeta;name=arm-platforms-extra-kmeta;destsuffix=arm-platforms-extra-kmeta"
SRC_URI:append = " ${SRC_URI_EXTRAS_KMETA}"

COMPATIBLE_MACHINE = "fvp-rd-kronos"

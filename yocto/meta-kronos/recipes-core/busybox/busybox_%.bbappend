#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

# As Bats requires the nl utility, configure CONFIG_NL=y for busybox if we are
# using Bats
SRC_URI:append:fvp-rd-kronos = "file://nl.cfg"

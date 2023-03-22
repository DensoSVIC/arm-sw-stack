#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# These patches are applied to both xen and xen-tools on the
# fvp-rd-kronos MACHINE
FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
SRC_URI += "\
    file://0001-tools-xendomains-Restrict-domid-pattern-in-LIST_GREP.patch \
    file://0002-tools-xendomains-Only-save-restore-migrate-if-suppor.patch \
    "

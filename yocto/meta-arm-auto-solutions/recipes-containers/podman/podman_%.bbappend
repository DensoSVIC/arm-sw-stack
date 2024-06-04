#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append:domu = " file://containers.conf"

do_install:append:domu () {
    install -d "${D}${sysconfdir}/containers"
    install -m 0644 "${WORKDIR}/containers.conf" "${D}${sysconfdir}/containers"
}

FILES:${PN}:append:domu = " ${sysconfdir}/containers/containers.conf"

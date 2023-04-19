#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += "\
    file://interfaces.d/0001-external \
    file://interfaces.d/0002-safety-island-c0 \
    file://interfaces.d/0002-safety-island-c1 \
    file://interfaces.d/0002-safety-island-c2 \
    "

install_interfaces() {
    cp -rf ${WORKDIR}/interfaces.d ${D}${sysconfdir}/network/
}
do_install[postfuncs] += "install_interfaces"

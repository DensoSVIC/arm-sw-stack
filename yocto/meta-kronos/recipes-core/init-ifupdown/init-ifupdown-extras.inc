#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += "\
    ${@bb.utils.contains('EXTRA_IMAGE_FEATURES', \
                         'baremetal', \
                         'file://baremetal/interfaces.d/', '', d)} \
    ${@bb.utils.contains('EXTRA_IMAGE_FEATURES', \
                         'virtualization', \
                         'file://virtualization/interfaces.d/', '', d)} \
    ${@bb.utils.contains('EXTRA_IMAGE_FEATURES', \
                         'domu', \
                         'file://domu/interfaces.d/', '', d)} \
    "

install_interfaces() {
    cp -rf ${WORKDIR}/*/interfaces.d ${D}${sysconfdir}/network/
}
do_install[postfuncs] += "install_interfaces"

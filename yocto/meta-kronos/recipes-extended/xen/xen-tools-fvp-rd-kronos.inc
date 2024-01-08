#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend := "${THISDIR}/files/xen:"

# /* cSpell:disable */
SRC_URI:remove = "file://0001-python-pygrub-pass-DISTUTILS-xen-4.15.patch"
SRC_URI:remove = "file://0001-tools-xenstore-xenstored_control.c-correctly-print-t.patch"
SRC_URI:append = " file://0001-python-pygrub-pass-DISTUTILS-xen-4.18.patch"
# /* cSpell:enable */

do_install:append(){
    # We want to make sure that the domain will be "dead" when "xl shutdown" is called
    sed -i 's#XENDOMAINS_SHUTDOWN="--wait"#XENDOMAINS_SHUTDOWN="--wait --wait"#' ${D}${sysconfdir}/default/xendomains
}


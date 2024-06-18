#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "A Linux kernel module for Trusted Services"
DESCRIPTION = "A Linux kernel module providing user space access to Trusted Services"
HOMEPAGE = "https://trusted-services.readthedocs.io/"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=05e355bbd617507216a836c56cf24983"

SRC_URI = "git://gitlab.arm.com/linux-arm/linux-trusted-services;protocol=https;branch=main \
           file://Makefile;subdir=git \
          "

# Tag tee-v1.1.2
SRCREV = "8a81f5d2406f146b15a705d49b256efaa5fa3ba9"

S = "${WORKDIR}/git"
COMPATIBLE_HOST = "(arm|aarch64).*-linux"
KERNEL_MODULE_AUTOLOAD += "arm-ffa-tee"
HEADER_FILENAME = "arm_ffa_tee.h"

inherit module

do_install:append() {
    install -d ${D}${includedir}
    install -m 0644 ${S}/uapi/${HEADER_FILENAME} ${D}${includedir}/
}

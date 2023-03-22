#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "Xen EFI config file"
DESCRIPTION = "Deploy a .cfg file that can be used to boot Xen using EFI"

inherit deploy features_check
REQUIRED_IMAGE_FEATURES += "virtualization"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "\
    file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302 \
    "
SRC_URI = "file://xen.cfg"

do_patch[noexec] = "1"
do_configure[noexec] = "1"
do_compile[noexec] = "1"
do_install[noexec] = "1"

do_deploy() {
    cp ${WORKDIR}/xen.cfg ${DEPLOYDIR}/xen.cfg
}
addtask deploy after do_install before do_build

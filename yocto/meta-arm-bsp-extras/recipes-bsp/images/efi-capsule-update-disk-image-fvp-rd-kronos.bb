#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "A disk image that contains an UEFI update capsule"
DESCRIPTION = "A genimage based image recipe which is used to generate a disk image which \
               contains a single vfat partition with an UEFI update capsule."
LICENSE = "MIT"
# License file is in "layers/poky/meta/files/common-licenses".
# nooelint: oelint.var.licenseremotefile
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
COMPATIBLE_MACHINE = "fvp-rd-kronos"

DEPENDS += "mtools-native"
SRC_URI = "file://${GENIMAGE_CONFIG}"

inherit genimage

do_genimage[depends] += "firmware-${MACHINE}:do_deploy"
GENIMAGE_CONFIG = "efi-capsule-update-disk-image.cfg"
GENIMAGE_VARIABLES[DEPLOY_DIR_IMAGE] = "${DEPLOY_DIR_IMAGE}"

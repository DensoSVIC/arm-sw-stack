#
# Based on: https://git.yoctoproject.org/meta-zephyr/tree/meta-zephyr-core/recipes-devtools/zephyr-sdk/zephyr-sdk_0.16.1.bb
# In open-source project: meta-zephyr
# Original file: SPDX-FileCopyrightText: <text>Copyright 2023 OpenEmbedded
# Contributors</text>
# Modifications: SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited
# and/or its affiliates <open-source-office@arm.com></text>
# SPDX-License-Identifier: MIT

# Changes:
# 1) Add HOMEPAGE
# 2) Add COMPATIBLE_MACHINE
# 3) Add oelint-adv suppressions
#
SUMMARY = "Zephyr SDK Bundle"
DESCRIPTION = "Official SDK built using crosstool-ng, distributed by the \
Zephyr project"
HOMEPAGE = "https://github.com/zephyrproject-rtos/sdk-ng/releases"
COMPATIBLE_HOST = "(x86_64|aarch64).*-linux"

# Installing the SDK on the target is not supported
COMPATIBLE_MACHINE:class-target = "unset"

LICENSE = "Apache-2.0"

# License file is in "layers/poky/meta/files/common-licenses".
# nooelint: oelint.var.licenseremotefile
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

INHIBIT_DEFAULT_DEPS = "1"
# CMake is required by the setup script
DEPENDS += "cmake"

SDK_ARCHIVE = "zephyr-sdk-${PV}_linux-${BUILD_ARCH}.tar.xz"
SDK_NAME = "${BUILD_ARCH}"
# SRC_URI checksum name is dynamic
# nooelint: oelint.vars.srcurichecksum
SRC_URI = "https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v${PV}/${SDK_ARCHIVE};subdir=${S};name=${SDK_NAME}"

SRC_URI[x86_64.sha256sum] = "51338d51aa4cea2516641ce0d9dc0b51b763779f00dc4564a2bc0dd713df22c7"
SRC_URI[aarch64.sha256sum] = "062bb2b5c47ca56dd29b7f92dd7f07a5ce22ba513759d2b6960bc658531eb00c"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

ZEPHYR_SDK_DIR = "${prefix}/zephyr-sdk"

do_install() {
    install -d ${D}${prefix}
    cp -r ${S}/zephyr-sdk-${PV} ${D}${ZEPHYR_SDK_DIR}

    # Install host tools
    ${D}${ZEPHYR_SDK_DIR}/setup.sh -h
}

SYSROOT_DIRS += "${ZEPHYR_SDK_DIR}"
INHIBIT_SYSROOT_STRIP = "1"
BBCLASSEXTEND = "native"

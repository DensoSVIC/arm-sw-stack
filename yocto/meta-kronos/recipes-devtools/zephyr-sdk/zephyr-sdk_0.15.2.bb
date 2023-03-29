# Based on: meta-zephyr-core/recipes-devtools/zephyr-sdk/zephyr-sdk_0.15.2.bb?id=c2d10c62c77f97d0cd3da571f91b8eec2aae20be
# In open-source project: https://git.yoctoproject.org/meta-zephyr
# Original file: SPDX-FileCopyrightText: Copyright 2023 Yocto Community
# Modifications: SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited
# and/or its affiliates <open-source-office@arm.com></text>
# SPDX-License-Identifier: MIT

SUMMARY = "Zephyr SDK Bundle"
DESCRIPTION = "Official SDK built using crosstool-ng, distributed by the \
Zephyr project"
COMPATIBLE_HOST = "(x86_64|aarch64).*-linux"

LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

INHIBIT_DEFAULT_DEPS = "1"
# CMake is required by the setup script
DEPENDS += "cmake"

SDK_ARCHIVE = "zephyr-sdk-${PV}_linux-${BUILD_ARCH}.tar.gz"
SDK_NAME = "${BUILD_ARCH}"
SRC_URI = "https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v${PV}/${SDK_ARCHIVE};subdir=${S};name=${SDK_NAME}"

SRC_URI[x86_64.sha256sum] = "8e3572fbca9f9ba18a4436c00d680af34a85e239f7fe66c7988da85571a0d23d"
SRC_URI[aarch64.sha256sum] = "0d36e40a81a3aa1b64d65c4c5c6cd381555a33ef1684b7ba98236fa486f176a4"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

# Installing the SDK on the target is not supported
COMPATIBLE_MACHINE:class-target = "unset"

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

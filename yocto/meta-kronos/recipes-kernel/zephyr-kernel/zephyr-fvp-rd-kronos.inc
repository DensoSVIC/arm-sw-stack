#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

ZEPHYR_APP ??= "helloworld"
require recipes-kernel/zephyr-kernel/zephyr-${ZEPHYR_APP}.bb

FILESEXTRAPATHS:prepend := "\
${THISDIR}/files:\
${KRONOS_REPO_DIRECTORY}/components/safety_island/zephyr:\
"

SRC_URI_ZEPHYR_EXTRA_PATCHES ?= "\
    file://zephyr/0001-include-arch-add-macro-for-mpu-memory-configuration.patch;patchdir=zephyr \
    file://zephyr/0002-soc-fvp_aemv8r-add-mpu-region-from-device-tree.patch;patchdir=zephyr \
    file://zephyr/0003-soc-include-fvp_aemv8r-Define-device-memory-as-devic.patch;patchdir=zephyr \
    "
SRC_URI += "${SRC_URI_ZEPHYR_EXTRA_PATCHES}"

# Add the safety_island Zephyr module
SRC_URI:append = " file://src"
ZEPHYR_SAFETY_ISLAND_MODULE = "${WORKDIR}/src"
ZEPHYR_MODULES:append = "${ZEPHYR_SAFETY_ISLAND_MODULE}\;"

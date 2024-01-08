#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

# /* cSpell:disable */
SRC_URI_ZEPHYR_EXTRA_PATCHES ?= "\
    file://zephyr/0001-soc-fvp_aemv8r-Flash-mpu-region-can-t-be-set-in-case.patch;patchdir=zephyr \
    file://zephyr/0002-shell-Provide-a-configuration-to-pin-the-shell-threa.patch;patchdir=zephyr \
    file://zephyr/0003-net-ip-Clarify-highest-priority-traffic-class.patch;patchdir=zephyr \
    file://zephyr/0004-net-gptp-Invert-priority-of-outgoing-packets.patch;patchdir=zephyr \
    file://zephyr/0005-net-ethernet-Don-t-use-VLAN-tag-on-gPTP-messages.patch;patchdir=zephyr \
    file://zephyr/0006-net-gptp-Create-a-stack-size-Kconfig-option.patch;patchdir=zephyr \
    file://zephyr/0007-drivers-gic-Add-multiple-GIC-redistributors-regions-.patch;patchdir=zephyr \
    file://zephyr/0008-lib-posix-Support-realtime-timer.patch;patchdir=zephyr \
    file://zephyr/0009-mbedtls-Enable-missing-algorithms-for-psa-arch-tests.patch;patchdir=zephyr \
    file://zephyr/0010-mbedtls-Add-rss-driver-to-build.patch;patchdir=zephyr \
    "
# /* cSpell:enable */

SRC_URI += "${SRC_URI_ZEPHYR_EXTRA_PATCHES}"

# Add the safety_island Zephyr module
ZEPHYR_SAFETY_ISLAND_MODULE = "${KRONOS_REPO_DIRECTORY}/components/safety_island/zephyr/src"
ZEPHYR_MODULES:append = "${ZEPHYR_SAFETY_ISLAND_MODULE}\;"
do_configure[file-checksums] += "${ZEPHYR_SAFETY_ISLAND_MODULE}:True"

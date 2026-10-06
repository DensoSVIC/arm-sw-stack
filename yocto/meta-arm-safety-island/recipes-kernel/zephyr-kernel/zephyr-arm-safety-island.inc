#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

require zephyr-tfm-psa-apis.inc
require zephyr-mbedtls-psa-apis.inc
require zephyr-psa-arch-tests.inc

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

# /* cSpell:disable */
SRC_URI_ZEPHYR_EXTRA_PATCHES ?= "\
    file://zephyr/0001-soc-fvp_aemv8r-Flash-mpu-region-can-t-be-set-in-case.patch;patchdir=zephyr \
    file://zephyr/0002-net-ip-Clarify-highest-priority-traffic-class.patch;patchdir=zephyr \
    file://zephyr/0003-net-gptp-Invert-priority-of-outgoing-packets.patch;patchdir=zephyr \
    file://zephyr/0004-net-ethernet-Don-t-use-VLAN-tag-on-gPTP-messages.patch;patchdir=zephyr \
    file://zephyr/0005-net-gptp-Create-a-stack-size-Kconfig-option.patch;patchdir=zephyr \
    file://zephyr/0006-drivers-gic-Add-multiple-GIC-redistributors-regions-.patch;patchdir=zephyr \
    file://zephyr/0007-lib-posix-Support-realtime-timer.patch;patchdir=zephyr \
    file://zephyr/0008-mbedtls-Enable-missing-algorithms-for-psa-arch-tests.patch;patchdir=zephyr \
    file://zephyr/0009-mbedtls-Add-rss-driver-to-build.patch;patchdir=zephyr \
    file://zephyr/0010-arch-arm-cortex-r-Add-compiler-tuning-for-Cortex-R82.patch;patchdir=zephyr \
    file://zephyr/0011-net-zperf-Add-TCP-and-UDP-receivers-stack-size-Kconf.patch;patchdir=zephyr \
    file://zephyr/0012-kernel-spinlock-Ticket-spinlocks.patch;patchdir=zephyr \
    file://zephyr/0013-net-iface-Introduce-TX-mutex-locking.patch;patchdir=zephyr \
    "
# /* cSpell:enable */

SRC_URI += "${SRC_URI_ZEPHYR_EXTRA_PATCHES}"

# Add the safety_island Zephyr module
ZEPHYR_SAFETY_ISLAND_MODULE = "${ARM_AUTO_SOLUTIONS_REPO_DIRECTORY}/components/safety_island/zephyr/src"
ZEPHYR_MODULES:append = "${ZEPHYR_SAFETY_ISLAND_MODULE}\;"
do_configure[file-checksums] += "${ZEPHYR_SAFETY_ISLAND_MODULE}:True"

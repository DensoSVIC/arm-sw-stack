#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend := "\
${THISDIR}/files:\
${KRONOS_REPO_DIRECTORY}/components/safety_island/zephyr:\
"

# /* cSpell:disable */
SRC_URI_ZEPHYR_EXTRA_PATCHES ?= "\
    file://zephyr/0001-soc-fvp_aemv8r-Flash-mpu-region-can-t-be-set-in-case.patch;patchdir=zephyr \
    file://zephyr/0002-net-config-Add-VLAN-identifier-as-network-initial-co.patch;patchdir=zephyr \
    file://zephyr/0003-net-eth-Fix-ethernet-max-header-size-on-VLAN-presenc.patch;patchdir=zephyr \
    file://zephyr/0004-shell-Provide-a-configuration-to-pin-the-shell-threa.patch;patchdir=zephyr \
    file://zephyr/0005-net-zperf-Add-support-for-bind-to-host-option-for-tc.patch;patchdir=zephyr \
    file://zephyr/0006-drivers-eth_smsc91x-Implement-promiscuous-mode.patch;patchdir=zephyr \
    file://zephyr/0007-net-sockets-Fix-connected-datagram-socket-packet-fil.patch;patchdir=zephyr \
    file://zephyr/0008-kernel-poll-Lock-all-calls-to-signal_poll_event.patch;patchdir=zephyr \
    file://zephyr/0009-net-sockets-Keep-lock-when-notifying-condvar.patch;patchdir=zephyr \
    "
# /* cSpell:enable */

SRC_URI += "${SRC_URI_ZEPHYR_EXTRA_PATCHES}"

# Add the safety_island Zephyr module
SRC_URI:append = " file://src"
ZEPHYR_SAFETY_ISLAND_MODULE = "${WORKDIR}/src"
ZEPHYR_MODULES:append = "${ZEPHYR_SAFETY_ISLAND_MODULE}\;"

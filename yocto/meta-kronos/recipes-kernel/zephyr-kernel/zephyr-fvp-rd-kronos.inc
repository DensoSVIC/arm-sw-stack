#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

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
    file://zephyr/0010-net-gptp-fix-follow_up-message-timestamp.patch;patchdir=zephyr \
    file://zephyr/0011-net-gptp-fix-follow_up-message-correction_field.patch;patchdir=zephyr \
    file://zephyr/0012-net-gptp-fix-announce-message-byte-order.patch;patchdir=zephyr \
    file://zephyr/0013-net-ip-Clarify-highest-priority-traffic-class.patch;patchdir=zephyr \
    file://zephyr/0014-net-gptp-Invert-priority-of-outgoing-packets.patch;patchdir=zephyr \
    file://zephyr/0015-net-ethernet-Don-t-use-VLAN-tag-on-gPTP-messages.patch;patchdir=zephyr \
    file://zephyr/0016-net-gptp-Create-a-stack-size-Kconfig-option.patch;patchdir=zephyr \
    file://zephyr/0017-drivers-gic-Add-multiple-GIC-redistributors-regions-.patch;patchdir=zephyr \
    file://zephyr/0018-lib-posix-Support-realtime-timer.patch;patchdir=zephyr \
    "
# /* cSpell:enable */

SRC_URI += "${SRC_URI_ZEPHYR_EXTRA_PATCHES}"

# Add the safety_island Zephyr module
ZEPHYR_SAFETY_ISLAND_MODULE = "${KRONOS_REPO_DIRECTORY}/components/safety_island/zephyr/src"
ZEPHYR_MODULES:append = "${ZEPHYR_SAFETY_ISLAND_MODULE}\;"
do_configure[file-checksums] += "${ZEPHYR_SAFETY_ISLAND_MODULE}:True"

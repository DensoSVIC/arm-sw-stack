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
    file://zephyr/0001-include-arch-add-macro-for-mpu-memory-configuration.patch;patchdir=zephyr \
    file://zephyr/0002-soc-fvp_aemv8r-add-mpu-region-from-device-tree.patch;patchdir=zephyr \
    file://zephyr/0003-soc-include-fvp_aemv8r-Define-device-memory-as-devic.patch;patchdir=zephyr \
    file://zephyr/0004-soc-fvp_aemv8r-Flash-mpu-region-can-t-be-set-in-case.patch;patchdir=zephyr \
    file://zephyr/0005-net-zperf-add-configurable-for-the-zperf-sessions-nu.patch;patchdir=zephyr \
    file://zephyr/0006-net-zperf-unify-get_session-among-TCP-and-UDP.patch;patchdir=zephyr \
    file://zephyr/0007-net-zperf-fix-typo-in-error-messages.patch;patchdir=zephyr \
    file://zephyr/0008-net-zperf-improve-code-quality-for-tcp_received-func.patch;patchdir=zephyr \
    file://zephyr/0009-net-zperf-Reduce-the-scope-of-input-address-variable.patch;patchdir=zephyr \
    file://zephyr/0010-net-zperf-allow-TCP-receiver-to-handle-multiple-conn.patch;patchdir=zephyr \
    file://zephyr/0011-net-ip-Kconfig-for-TCP-packet-allocation-timeout.patch;patchdir=zephyr \
    file://zephyr/0012-drivers-phy_mii-Remove-the-unneeded-header-file.patch;patchdir=zephyr \
    file://zephyr/0013-kernel-allow-for-arch-specific-processing-within-bus.patch;patchdir=zephyr \
    file://zephyr/0014-drivers-gic-Add-irq-pending-check-and-clear-function.patch;patchdir=zephyr \
    file://zephyr/0015-arm64-prevent-possible-deadlock-on-SMP-with-FPU-shar.patch;patchdir=zephyr \
    file://zephyr/0016-test-kernel-fpu_sharing-Fix-text-exception-by-increa.patch;patchdir=zephyr \
    file://zephyr/0017-arch-arm64-mpu-Remove-LOG-print-before-mpu-enabled.patch;patchdir=zephyr \
    file://zephyr/0018-arch-arm64-Fix-the-STACK_INIT-logic-during-the-reset.patch;patchdir=zephyr \
    file://zephyr/0019-arch-arm64-Enable-FPU-and-FPU_SHARING-for-v8r-aarch6.patch;patchdir=zephyr \
    file://zephyr/0020-arch-arm64-Save-SIMD-registers-on-interrupts.patch;patchdir=zephyr \
    file://zephyr/0021-net-config-Add-VLAN-identifier-as-network-initial-co.patch;patchdir=zephyr \
    file://zephyr/0022-net-eth-Fix-ethernet-max-header-size-on-VLAN-presenc.patch;patchdir=zephyr \
    file://zephyr/0023-shell-Provide-a-configuration-to-pin-the-shell-threa.patch;patchdir=zephyr \
    file://zephyr/0024-net-zperf-Add-support-for-bind-to-host-option-for-tc.patch;patchdir=zephyr \
    "
# /* cSpell:enable */

SRC_URI += "${SRC_URI_ZEPHYR_EXTRA_PATCHES}"

# Add the safety_island Zephyr module
SRC_URI:append = " file://src"
ZEPHYR_SAFETY_ISLAND_MODULE = "${WORKDIR}/src"
ZEPHYR_MODULES:append = "${ZEPHYR_SAFETY_ISLAND_MODULE}\;"

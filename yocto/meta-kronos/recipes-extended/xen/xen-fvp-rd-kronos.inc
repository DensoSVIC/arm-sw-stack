#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# These patches are applied to both xen and xen-tools on the
# fvp-rd-kronos MACHINE
FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

# /* cSpell:disable */
SRC_URI += "\
    file://0001-tools-xendomains-Only-save-restore-migrate-if-suppor.patch \
    file://0002-arm-cpufeature-discover-CPU-support-for-MPAM.patch \
    file://0003-xen-arm-Set-default-value-of-MPAM-system-registers.patch \
    file://0004-xen-arm-Introduce-a-Kconfig-to-enable-MPAM.patch \
    file://0005-docs-device-tree-Add-MPAM-device-tree-binding.patch \
    file://0006-xen-arm-Introduce-detection-of-MPAM-Memory-System-Co.patch \
    file://0007-xen-common-arm-Move-get_logical_cpu_from_hw_id-to-sm.patch \
    file://0008-xen-arm-mpam-Initialise-MSC-from-firmware.patch \
    file://0009-xen-drivers-arm-Move-macros-FIELD_PREP-and-FIELD_GET.patch \
    file://0010-xen-arm-sysregs-Initialize-maximum-PARTID-and-PMG-fr.patch \
    file://0011-xen-arm-mpam-Introduce-MPAM-resources.patch \
    file://0012-xen-arm-mpam-Create-RIS-from-firmware.patch \
    file://0013-xen-arm-mpam-Add-MPAM-memory-mapped-register-definit.patch \
    file://0014-xen-arm-mpam-Probe-the-MSC-hardware-at-CPU-online-st.patch \
    file://0015-xen-arm-mpam-Probe-the-MPAM-hardware-features-suppor.patch \
    file://0016-xen-arm-mpam-Reset-MSC-and-RIS-controls-during-CPU-o.patch \
    file://0017-xen-arm-mpam-Enable-MPAM-at-boot-time.patch \
    file://0018-xen-arm-mpam-Merge-supported-features-into-mpam_clas.patch \
    file://0019-xen-arm-mpam-Introduce-mpam_config-for-each-PARTID.patch \
    file://0020-xen-arm-mpam-Add-MPAM-Xen-cmdline-with-related-parsi.patch \
    file://0021-xen-arm-Apply-MPAM-config-to-Dom0.patch \
    file://0022-xen-arm-Apply-MPAM-config-to-dom0less-domUs.patch \
    file://0023-xen-arm-Context-switch-MPAM-system-registers-between.patch \
    file://0024-xen-arm-Set-domain-default-PARTID-and-PMG.patch \
    file://0025-tools-xl-psr-Split-display-name-for-arch-specific-te.patch \
    file://0026-tools-xl-psr-Expose-xl-psr-to-Arm64.patch \
    file://0027-xen-arm-Add-dummy-xl-psr-related-domctl-and-sysctl-f.patch \
    file://0028-xen-arm-Parse-cache-information-from-device-tree-at-.patch \
    file://0029-xen-arm-Implement-MPAM-sysctls-for-L3-cache.patch \
    file://0030-xen-arm-Return-correct-cores_per_socket-value-in-XEN.patch \
    file://0031-xen-arm-Implement-MPAM-domctls-for-L3-cache.patch \
    file://0032-xen-arm-doc-Add-MPAM-system-level-cache-SLC-control.patch \
    file://0033-tools-xl-psr-Consolidate-CAT-hwinfo-to-a-single-help.patch \
    file://0034-xen-arm-sysctl-Add-SLC-related-PSR-sysctl-subops.patch \
    file://0035-tools-xl-psr-Support-SLC-in-xl-psr-hwinfo-cmd.patch \
    file://0036-xen-arm-domctl-Add-SLC-related-PSR-domctl-subops.patch \
    file://0037-tools-xl-psr-Support-SLC-in-xl-psr-cat-show-set-comm.patch \
    file://0038-xen-arm-Allow-MPAM-v1.1-in-MSC-hardware-probing-proc.patch \
    "
# /* cSpell:enable */

SRC_URI:append = " file://rd-kronos.cfg"

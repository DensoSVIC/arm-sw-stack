#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

PV .= "+git${SRCPV}"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
# cSpell:disable
SRC_URI += "\
    file://0001-scp-firmware-Abstract-scp-firmware-common-code-for-a.patch \
    file://0002-product-Add-Kronos-support.patch \
    file://0003-cmn_cyprus-Add-cmn_hns_cfg_ctl-to-HN-S-register-layo.patch \
    file://0004-cmn_cyprus-Enable-MPAM-CPOR-and-CCAP-for-SLC.patch \
    file://0005-product-rdkronos-Add-MHU3-configuration-for-SCP-to-R.patch \
    file://0006-product-rdkronos-Add-PPU-operations-for-SI-clusters.patch \
    file://0007-product-rdkronos-Use-correct-part-number-for-kronos.patch \
    file://0008-product-rdkronos-Use-correct-ATU-attributes-for-NCI-.patch \
    file://0009-product-rdkronos-Fix-unaligned-access-to-NCI-nodes.patch \
    file://0010-product-rdkronos-Set-correct-BL2-start-address-as-AP.patch \
    file://0011-product-rdkronos-Remove-the-reserved-AP-Trusted-SRAM.patch \
    file://0012-product-rdkronos-Add-AP-and-SI-shared-SRAM-to-CMN-me.patch \
    file://0013-product-rdkronos-Enable-PCIe-x1-x2-x2.1-and-x4-confi.patch \
    file://0014-product-rdkronos-Differentiate-between-reset-and-shu.patch \
    file://0015-product-rdkronos-Remove-unused-PCIe-configuration.patch \
    file://0016-product-rdkronos-Set-log-level-to-ERROR.patch \
    file://0017-product-rdkronos-Increase-the-ATU-request-timeout.patch \
"
# cSpell:enable

COMPATIBLE_MACHINE     = "fvp-rd-kronos"
SCP_BUILD_RELEASE      = "0"
SCP_PLATFORM           = "rdkronos"
SCP_LOG_LEVEL          = "INFO"
FW_TARGETS             = "scp lcp"
FW_INSTALL             = "ramfw"

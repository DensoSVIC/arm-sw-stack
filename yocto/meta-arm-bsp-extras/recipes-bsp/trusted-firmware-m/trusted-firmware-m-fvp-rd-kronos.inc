#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# fvp-rd-kronos machines specific TFM support

COMPATIBLE_MACHINE = "(fvp-rd-kronos)"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
# cSpell:disable
SRC_URI += "\
    file://0001-RSS-Abstract-RSS-common-code-for-all-platforms.patch \
    file://0002-Add-new-platform-called-kronos-which-is-based-on-rdf.patch \
    file://0003-kronos-Replace-RSS-flash-with-SI-NVM-flash.patch \
    file://0004-rss-kronos-Add-support-of-booting-3-SI-clusters.patch \
    file://0005-rss-kronos-Remove-MCP-from-kronos-platform.patch \
    file://0006-rss-kronos-Boot-AP-from-BL2.patch \
    file://0007-PS-ITS-Remove-PS-dependency-on-ITS.patch \
    file://0008-RSS-Add-PS-NV-counters-in-OTP-for-Kronos-platform.patch \
    file://0009-RSS-Fix-bug-with-PS-OTP-counters-option.patch \
    file://0010-RSS-Enable-the-communication-between-AP-and-RSS.patch \
    file://0011-RSS-Enable-Protected-Storage-service-for-kronos.patch \
    file://0012-RSS-Increase-the-maximum-payload-size-and-change-mai.patch \
    file://0013-rss-kronos-Decrease-SI-CL0-LLRAM-size-to-2MB.patch \
    file://0014-RSS-Crypto-Service-allow-permission-check-for-PSA_IP.patch \
    file://0015-RSS-Configure-flash-area-for-ITS.patch \
    file://0016-RSS-Enable-ITS-service.patch \
    file://0017-RSS-Enlarge-the-maximum-file-size-of-ITS.patch \
    file://0018-RSS-Make-hardcoded-MHUv3-driver-values-configurable.patch \
    file://0019-RSS-Kronos-Enable-MHUv3-communication-between-RSS-an.patch \
    file://0020-RSS-Kronos-Add-and-configure-ATU-regions-between-Saf.patch \
    file://0021-RSS-Kronos-Enable-ITS-service-permission-check.patch \
    file://0022-rss-driver-Add-GIC-multiple-views-support-for-Kronos.patch \
    file://0023-platform-rss-kronos-Import-NCI-Tower-driver-from-RD-.patch \
    file://0024-rss-kronos-Kronos-board-specific-Tower-NCI-changes.patch \
    file://0025-rss-kronos-Use-RSS-flash-instead-of-SI-flash-to-stor.patch \
    file://0026-platform-rss-common-Introduce-NI-710-AE-APU-driver.patch \
    file://0027-platform-rss-kronos-Add-Kronos-board-specific-NI-710.patch \
    file://0028-bl2-Add-PCIe-memory-space-to-SCP-ATU-regions.patch \
    file://0029-rss-kronos-Handle-shutdown-requests-over-MHU.patch \
    file://0030-rss-kronos-Wait-until-secure-fw-is-ready-to-signal-S.patch \
    file://0031-rss-kronos-Map-SI-CL1-RSS-MHU-Interrupts-in-GIC-mult.patch \
    file://0032-RSS-Make-maximum-number-of-concurrent-requests-confi.patch \
    file://0033-rss-drivers-Fix-setting-of-id_valid-in-the-in-NI-710.patch \
    file://0034-rss-kronos-Improvements-for-asni_rss_mm-APU-configs.patch \
    file://0035-rss-kronos-Fix-the-SI-PC-shared-memory-APU-config.patch \
    file://0036-platform-Disable-interrupts-in-stdio-output-string-f.patch \
    file://0037-platform-RSS-Block-mailbox-IRQs-while-replying-on-MH.patch \
    file://0038-rss-kronos-Add-GPT-driver-for-kronos-platform.patch \
    file://0039-rss-kronos-Add-TFM_FWU_AGENT-macro-and-misc-changes-.patch \
    file://0040-rss-kronos-Add-fwu-agent-for-kronos-platform.patch \
    file://0041-rss-kronos-Provision-metadata-private-metadata-to-RS.patch \
    file://0042-rss-kronos-Add-the-AP-Secure-Flash-for-FWU.patch \
    file://0043-rss-kronos-Split-fwu_agent.c-and-refactor-metadata-c.patch \
    file://0044-rss-kronos-Parse-GPT-partition-for-AP-secure-flash.patch \
    file://0045-rss-kronos-Provision-AP-metadata-and-parse-FIP-image.patch \
    file://0046-rss-kronos-Add-PSA-IOCTL-API-and-flash-capsule-image.patch \
    file://0047-rss-kronos-Handle-host-ack-and-fmp-image-info-reques.patch \
    file://0048-rss-kronos-Change-capusle-image-guid.patch \
    file://0049-rss-kronos-Increment-ITS-number-of-assets.patch \
    file://0050-rss-kronos-Add-common-platform-logger.patch \
    file://0051-rss-kronos-Use-platform-logger-for-Firmware-Update.patch \
    file://0052-RSS-Make-Tower-NCI-and-NI710AE-tree-printing-configu.patch \
    file://0053-rss-kronos-Add-SI-RSS-ATU-mapping.patch \
    file://0054-rss-kronos-Add-FVP-shutdown-handler.patch \
    file://0055-rss-kronos-Remove-unused-PCIe-address-mapping.patch \
    file://0056-rss-kronos-Remove-LCP.patch \
    file://0057-rss-kronos-Reduce-the-number-of-used-RSS-SCP-MHUv3-c.patch \
    file://0058-rss-driver-Power-on-redistributor-for-GIC-View-0.patch \
    file://0059-Kronos-Enlarge-the-payload-size-of-rss_comms-message.patch \
    file://0060-Platform-Kronos-Increase-buffers-for-EFI-vars.patch \
    file://0061-Platform-Kronos-Power-on-all-the-Fainlight-GIC-redis.patch \
    file://0062-rss-driver-Fix-computation-of-GIC-IVIEWRn.patch \
    file://0063-Platform-Kronos-Configure-RSS-MPU.patch \
    file://0064-Fix-Increased-variable-sizes-for-assets-and-crypt-bu.patch \
    file://0065-SPM-Record-schedule-attempt-in-PendSV-if-scheduler-i.patch \
    file://0066-rss-kronos-Enable-signed-capsule.patch \
    file://0067-RSS-Fix-coding-issues.patch \
"
# cSpell:enable

TFM_DEBUG = "1"
TFM_PLATFORM = "arm/rss/kronos"

EXTRA_OECMAKE += "-D ATU_SCP=ON"
EXTRA_OECMAKE += "-D RSS_USE_HOST_FLASH=OFF"
EXTRA_OECMAKE += "-D ENABLE_SCP_ATU_CTRL=OFF"
EXTRA_OECMAKE += "-D RSS_USE_SI_FLASH=ON"
EXTRA_OECMAKE += "-D TFM_FWU_AGENT=ON"
EXTRA_OECMAKE += "-D TFM_SPM_LOG_LEVEL=TFM_SPM_LOG_LEVEL_INFO"
EXTRA_OECMAKE += "-D PLAT_LOG_LEVEL=PLAT_LOG_LEVEL_INFO"

# The RSS platform's cpak_generator uses a sub-invocation of CMake, using
# default arguments to pick up the host toolchain. To make this work, do not
# export the target toolchain in the CC environment variable. CMake can still
# find the target toolchain using the toolchain file.
CC[unexport] = "1"

# Only install the required binaries
do_install() {
    install -D -p -m 0644 ${B}/install/outputs/bl1_1.bin ${D}/firmware/bl1_1.bin
    install -D -p -m 0644 ${B}/install/outputs/encrypted_cm_provisioning_bundle_0.bin ${D}/firmware/encrypted_cm_provisioning_bundle_0.bin
    install -D -p -m 0644 ${B}/install/outputs/encrypted_dm_provisioning_bundle.bin ${D}/firmware/encrypted_dm_provisioning_bundle.bin
    install -D -p -m 0644 ${B}/install/outputs/bl2_signed.bin ${D}/firmware/bl2_signed.bin
    install -D -p -m 0644 ${B}/install/outputs/tfm_s_ns_signed.bin ${D}/firmware/tfm_s_ns_signed.bin
}

# TF-M is running on an M-core alongside the AArch64 A/R cores
INSANE_SKIP:${PN} += "arch"

inherit apply_local_src_patches
LOCAL_SRC_PATCHES_INPUT_DIR = "N/A"

do_apply_local_src_patches() {
    apply_local_src_patches ${S}/lib/ext/qcbor ${WORKDIR}/git/qcbor
    apply_local_src_patches ${S}/lib/ext/mbedcrypto ${WORKDIR}/git/mbedtls
}

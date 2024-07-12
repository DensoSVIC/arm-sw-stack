#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# FVP RD-Kronos specific U-boot support

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
# cSpell: disable
SRC_URI:append:fvp-rd-kronos = " \
        file://0001-misc-vexpress_config-Use-dev_set_uclass_priv-to-set-.patch \
        file://0002-vexpress64-Set-max-mmc-block-count-to-the-limitation.patch \
        file://0003-vexpress64-Add-MMC-card-to-the-BOOT_TARGET_DEVICES-o.patch \
        file://0004-arm-Move-sev-and-wfe-definitions-to-common-Arm-heade.patch \
        file://0005-vexpress64-Introduce-wfe-based-udelay-implementation.patch \
        file://0006-vexpress64-Add-SMMU-and-PCIe-memory-map-regions.patch \
        file://0007-serial-pl01x-Modify-pending-callback-to-test-if-tran.patch \
        file://0008-arm64-smccc-add-support-for-SMCCCv1.2-x0-x17-registe.patch \
        file://0009-lib-uuid-introduce-uuid_str_to_le_bin-function.patch \
        file://0010-lib-uuid-introduce-testcase-for-uuid_str_to_le_bin.patch \
        file://0011-arm_ffa-introduce-Arm-FF-A-support.patch \
        file://0012-arm_ffa-introduce-sandbox-FF-A-support.patch \
        file://0013-arm_ffa-introduce-sandbox-test-cases-for-UCLASS_FFA.patch \
        file://0014-arm_ffa-introduce-armffa-command.patch \
        file://0015-arm_ffa-efi-introduce-FF-A-MM-communication.patch \
        file://0016-virtio-rng-Workaround-for-FVP-returning-zero-size-bu.patch \
        file://0017-vexpress64-Set-the-DM_RNG-property.patch \
        file://0018-efi-corstone1000-fwu-introduce-EFI-capsule-update.patch \
        file://0019-arm-corstone1000-fix-unrecognized-filesystem-type.patch \
        file://0020-efi_loader-corstone1000-remove-guid-check-from-corst.patch \
        file://0021-efi_loader-populate-ESRT-table-if-EFI_ESRT-config-op.patch \
        file://0022-efi_firmware-add-get_image_info-for-corstone1000.patch \
        file://0023-efi_loader-fix-null-pointer-exception-with-get_image.patch \
        file://0024-arm-corstone1000-add-mmc-for-fvp.patch \
        file://0025-corstone1000-add-compressed-kernel-support.patch \
        file://0026-Introduce-external-sys-driver-to-device-tree.patch \
        file://0027-Add-mhu-and-rpmsg-client-to-u-boot-device-tree.patch \
        file://0028-arm-corstone1000-esrt-support.patch \
        file://0029-corstone1000-enable-distro-booting-command.patch \
        file://0030-corstone1000-add-fwu-metadata-store-info.patch \
        file://0031-fwu_metadata-make-sure-structures-are-packed.patch \
        file://0032-corstone1000-add-boot-index.patch \
        file://0033-corstone1000-adjust-boot-bank-and-kernel-location.patch \
        file://0034-corstone1000-add-nvmxip-fwu-mdata-and-gpt-options.patch \
        file://0035-nvmxip-move-header-to-include.patch \
        file://0036-corstone1000-set-kernel_addr-based-on-boot_idx.patch \
        file://0037-corstone1000-boot-index-from-active.patch \
        file://0038-corstone1000-enable-PSCI-reset.patch \
        file://0039-Enable-EFI-set-get-time-services.patch \
        file://0040-Enables-on-disk-capsule-update-feature.patch \
        file://0041-efi-vexpress64-Enable-capsule-update-get_image_info-.patch \
        file://0042-efi-vexpress64-Set-capsule-shared-buffer-base-addres.patch \
        file://0043-efi-Do-not-pack-struct-efi_firmware_image_descriptor.patch \
        file://0044-efi_loader-Add-PSCI_RESET-to-EFI_CAPSULE_ON_DISK-dep.patch \
        file://0045-efi_loader-Use-PSCI-reset-instead-of-system-reset-AP.patch \
        file://0046-efi-vexpress64-Change-EFI_FIRMWARE_IMAGE_TYPE_UBOOT_.patch \
        file://0047-vexpress64-Respect-Kronos-memory-map-for-the-second-.patch \
        file://0048-efi_loader-Add-Update-Capsule-flags-check.patch \
        file://kronos.cfg \
        "

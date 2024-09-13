#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SRCREV_tfa = "9881bb93a3bc0a3ea37e9f093e09ab4b360a9e48"
PV .= "+git${SRCPV}"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
# cSpell:disable
SRC_URI += "\
    file://rdkronos.dts;subdir=git/fdts \
    file://0001-drivers-arm-mhuv3-add-MHUv3-driver.patch \
    file://0002-TEMP-drivers-arm-add-function-to-trigger-mhuv3-doorb.patch \
    file://0003-feat-rdkronos-Introduce-Arm-RD-Kronos-platform.patch \
    file://0004-feat-rdkronos-Compile-and-load-the-HW_CONFIG-device-.patch \
    file://0005-feat-rdkronos-Introduce-FW_CONFIG-loading-for-BL2_AT.patch \
    file://0006-feat-rdkronos-Add-support-for-OP-TEE-SPMC.patch \
    file://0007-fix-avoid-cpu-off-message-to-FF-A-v1.0-SPMC.patch \
    file://0008-feat-rdkronos-Reduce-the-ram-size-for-OP-TEE-to-4M.patch \
    file://0009-feat-rdkronos-Enable-SVE_FOR_NS-and-SVE_FOR_SWD-flag.patch \
    file://0010-bug-rdkronos-Enable-GIC600-support.patch \
    file://0011-feat-rdkronos-Enable-FWU-feature-for-kronos-platform.patch \
    file://0012-feat-rdkronos-Add-FWU-related-configuration-in-rdkro.patch \
    file://0013-feat-scmi-Add-infinite-loop-to-handle-unsupported-po.patch \
    file://0014-psci-SMCCC_ARCH_FEATURES-discovery-through-PSCI_FEATURES.patch \
    file://0015-fix-rdkronos-Remove-tb_fw_configs-and-settings.patch \
    file://0016-feat-bl2-Add-support-for-loading-DTB-at-BL2.patch \
    file://0017-fix-rdkronos-Enabling-the-BL2_ENABLE_DTB_LOAD-option.patch \
    file://0018-feat-auth-Add-FW_CONFIG-to-BL2-Chain-of-Trust.patch \
    file://0019-feat-rdkronos-Enabling-Trusted-Board-Boot-TBB-for-rd.patch \
    "
# cSpell:enable

COMPATIBLE_MACHINE           = "fvp-rd-kronos"
TFA_PLATFORM                 = "rdkronos"
TFA_BUILD_TARGET             = "bl2 fip"
TFA_INSTALL_TARGET           = "bl2 fip"
TFA_DEBUG                    = "1"
TFA_MBEDTLS                  = "1"
TFA_UBOOT                    = "1"

TFA_SPD = "spmd"
TFA_SPMD_SPM_AT_SEL2 = "0"

# BL2 loads BL32 (optee). So, optee needs to be built first:
DEPENDS += "optee-os efitools-native"

# ENABLE_MPAM_FOR_LOWER_ELS is the TF-A make parameter to support Xen MPAM
EXTRA_OEMAKE:append = " \
    ARM_ARCH_MAJOR=9 ARM_ARCH_MINOR=0 \
    SVE_VECTOR_LEN=128 \
    ENABLE_MPAM_FOR_LOWER_ELS=1 \
    TRUSTED_BOARD_BOOT=1 \
    GENERATE_COT=1 \
    COT=tbbr \
    ARM_ROTPK_LOCATION=devel_rsa \
    ROT_KEY=plat/arm/board/common/rotpk/arm_rotprivk_rsa.pem \
    BL32=${RECIPE_SYSROOT}/${nonarch_base_libdir}/firmware/tee-pager_v2.bin \
    ${@'' if d.getVar('SVE_DISABLE_FLAG', True) == '1' else 'ARM_ARCH_MAJOR=9 ARM_ARCH_MINOR=2 SVE_VECTOR_LEN=128'} \
"

# This is needed to have the native tools executing properly by setting the RPATH
# correctly for TF-A 2.8.6
do_compile[prefuncs] += "fix_ldlibs gen_esl"
fix_ldlibs() {
    sed -i '/^LDLIBS/ s,$, \$\{BUILD_LDFLAGS},' ${S}/tools/fiptool/Makefile
}
gen_esl() {
    cert-to-efi-sig-list "${UEFI_SB_KEYS_DIR}/DB.crt" "${WORKDIR}/capsule.esl"
}

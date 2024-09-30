#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "The firmware images for fvp-rd-kronos"
DESCRIPTION = "A recipe to generate all the firmware images for fvp-rd-kronos \
using genimage."
LICENSE = "MIT"
# License file is in "layers/poky/meta/files/common-licenses".
# nooelint: oelint.var.licenseremotefile
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
COMPATIBLE_MACHINE = "fvp-rd-kronos"

DEPENDS += "\
    ${SAFETY_ISLAND_C0_RECIPE} \
    ${SAFETY_ISLAND_C1_RECIPE} \
    ${SAFETY_ISLAND_C2_RECIPE} \
    efitools-native \
    fiptool-native \
    scp-firmware \
    trusted-firmware-a \
    trusted-firmware-m \
"

SRC_URI = "file://${GENIMAGE_CONFIG}"

inherit genimage tfm_sign_image

# genimage configuration
GENIMAGE_CONFIG = "firmware.cfg"
GENIMAGE_VARIABLES[RECIPE_SYSROOT] = "${RECIPE_SYSROOT}"
GENIMAGE_VARIABLES[TFM_IMAGE_SIGN_DEPLOY_DIR] = "${TFM_IMAGE_SIGN_DEPLOY_DIR}"

# Image signing configuration
TFA_BL2_BINARY = "bl2.bin"
TFA_BL2_IMAGE_LOAD_ADDRESS = "0x70083C00"
TFA_BL2_SIGN_BIN_SIZE = "0x80000"

TFA_FIP_BINARY = "fip.bin"
TFA_FIP_IMAGE_LOAD_ADDRESS = "0x70090000"
TFA_FIP_SIGN_BIN_SIZE = "0x200000"

SCP_FIRMWARE_BINARY = "scp_ramfw.bin"
SCP_FIRMWARE_IMAGE_LOAD_ADDRESS = "0x70001c00"
SCP_FIRMWARE_SIGN_BIN_SIZE = "0x80000"

SI_CL0_FIRMWARE_BINARY = "${SAFETY_ISLAND_C0_IMAGE}.bin"
SI_CL0_FIRMWARE_IMAGE_LOAD_ADDRESS = "0x70105C00"
SI_CL0_FIRMWARE_SIGN_BIN_SIZE = "0x200000"

SI_CL1_FIRMWARE_BINARY = "${SAFETY_ISLAND_C1_IMAGE}.bin"
SI_CL1_FIRMWARE_IMAGE_LOAD_ADDRESS = "0x70307C00"
SI_CL1_FIRMWARE_SIGN_BIN_SIZE = "0x400000"

SI_CL2_FIRMWARE_BINARY = "${SAFETY_ISLAND_C2_IMAGE}.bin"
SI_CL2_FIRMWARE_IMAGE_LOAD_ADDRESS = "0x70709C00"
SI_CL2_FIRMWARE_SIGN_BIN_SIZE = "0x800000"

do_sign_images() {
    # Sign TF-A BL2
    sign_host_image ${RECIPE_SYSROOT}/firmware/${TFA_BL2_BINARY} \
        ${TFA_BL2_IMAGE_LOAD_ADDRESS} ${TFA_BL2_SIGN_BIN_SIZE}

    # Update BL2 in the FIP image
    cp ${RECIPE_SYSROOT}/firmware/${TFA_FIP_BINARY} ${TFM_IMAGE_SIGN_DIR}
    fiptool update --tb-fw \
        ${TFM_IMAGE_SIGN_DEPLOY_DIR}/signed_${TFA_BL2_BINARY} \
        ${TFM_IMAGE_SIGN_DIR}/${TFA_FIP_BINARY}

    # Sign the FIP image
    sign_host_image ${TFM_IMAGE_SIGN_DIR}/${TFA_FIP_BINARY} \
        ${TFA_FIP_IMAGE_LOAD_ADDRESS} ${TFA_FIP_SIGN_BIN_SIZE}

    # Sign SCP image
    sign_host_image ${RECIPE_SYSROOT}/firmware/${SCP_FIRMWARE_BINARY} \
        ${SCP_FIRMWARE_IMAGE_LOAD_ADDRESS} ${SCP_FIRMWARE_SIGN_BIN_SIZE}

    # Sign the Safety Island images
    # They may be the same file so create separate copies before signing
    cp ${RECIPE_SYSROOT}/firmware/${SI_CL0_FIRMWARE_BINARY} \
        ${B}/safety_island_c0.bin
    cp ${RECIPE_SYSROOT}/firmware/${SI_CL1_FIRMWARE_BINARY} \
        ${B}/safety_island_c1.bin
    cp ${RECIPE_SYSROOT}/firmware/${SI_CL2_FIRMWARE_BINARY} \
        ${B}/safety_island_c2.bin
    sign_host_image ${B}/safety_island_c0.bin \
        ${SI_CL0_FIRMWARE_IMAGE_LOAD_ADDRESS} ${SI_CL0_FIRMWARE_SIGN_BIN_SIZE}
    sign_host_image ${B}/safety_island_c1.bin \
        ${SI_CL1_FIRMWARE_IMAGE_LOAD_ADDRESS} ${SI_CL1_FIRMWARE_SIGN_BIN_SIZE}
    sign_host_image ${B}/safety_island_c2.bin \
        ${SI_CL2_FIRMWARE_IMAGE_LOAD_ADDRESS} ${SI_CL2_FIRMWARE_SIGN_BIN_SIZE}
}
do_sign_images[dirs] += "${B}"
do_sign_images[doc] = "Sign flash binaries using imgtool"
# Override task definition to ensure it runs before do_genimage
addtask sign_images after do_prepare_recipe_sysroot before do_genimage

# UEFI capsule configuration
CAPSULE_IMG_LOCATION = "${B}"
UEFI_FIRMWARE_BINARY = "efi-capsule-update-image.img"
CAPSULE_INDEX = "0"
CAPSULE_FW_VERSION = "3"
CAPSULE_GUID = "bcac8ebe-1128-40b3-a465-9e35f230324b"
CAPSULE_MONOTONIC_COUNT = "0"

MKEFICAPSULE_ARGS = "\
    --fw-version ${CAPSULE_FW_VERSION} \
    --guid ${CAPSULE_GUID} \
    --index ${CAPSULE_INDEX} \
    --monotonic-count ${CAPSULE_MONOTONIC_COUNT} \
"

do_uefi_capsule() {
    # Three capsule images are created:
    # fw.cap (signed), unsigned_fw.cap and tampered_fw.cap
    # These are for validation purposes. The unsigned and tampered
    # capsule images can be removed.

    # Create the unsigned update capsule
    mkeficapsule ${MKEFICAPSULE_ARGS} \
                 ${UEFI_FIRMWARE_BINARY} \
                 ${CAPSULE_IMG_LOCATION}/unsigned_${UEFI_FIRMWARE_BINARY}.uefi.capsule

    # Create the signed update capsule
    mkeficapsule ${MKEFICAPSULE_ARGS} \
                 --private-key "${UEFI_SB_KEYS_DIR}/DB.key" \
                 --certificate "${UEFI_SB_KEYS_DIR}/DB.crt" \
                 ${UEFI_FIRMWARE_BINARY} \
                 ${CAPSULE_IMG_LOCATION}/${UEFI_FIRMWARE_BINARY}.uefi.capsule

    # Truncate the last 5 bytes of the tampered capsule
    head -c -5 ${CAPSULE_IMG_LOCATION}/${UEFI_FIRMWARE_BINARY}.uefi.capsule \
        > ${CAPSULE_IMG_LOCATION}/tampered_${UEFI_FIRMWARE_BINARY}.uefi.capsule
    # Tamper the last 5 bytes.
    # Keep the size of payload to make sure the layouts of all images won't be
    # changed.
    echo 'BEEF' >> ${CAPSULE_IMG_LOCATION}/tampered_${UEFI_FIRMWARE_BINARY}.uefi.capsule
}
do_uefi_capsule[depends] += "u-boot-tools-native:do_populate_sysroot"
do_uefi_capsule[dirs] = "${B}"
do_uefi_capsule[doc] = "Generate UEFI capsule using mkeficapsule"
addtask uefi_capsule after do_genimage before do_deploy

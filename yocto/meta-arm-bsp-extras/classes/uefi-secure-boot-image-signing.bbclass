#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# UEFI Secure Boot signing image function

DEPENDS += "efitools-native sbsigntool-native"

sign_image() {
    local IMAGE_PATH="${1}"

    sbsign --key "${UEFI_SB_KEYS_DIR}/DB.key" --cert "${UEFI_SB_KEYS_DIR}/DB.crt" \
        --output ${IMAGE_PATH} ${IMAGE_PATH}
}

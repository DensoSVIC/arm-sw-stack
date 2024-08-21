#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

DEPENDS += "efitools-native sbsigntool-native"

SIGNATURE_LIST_GUID ?= "11111111-2222-3333-4444-123456789abc"
UEFI_SB_AUTH_VARS_NAME ?= "uefi-sb-authenticated-variables"
UEFI_SB_AUTH_VARS_DIR = "${B}/${UEFI_SB_AUTH_VARS_NAME}"

python generate_auth_vars() {
    import subprocess
    import os

    uefi_sb_keys_dir = d.getVar("UEFI_SB_KEYS_DIR")
    uefi_sb_auth_vars_dir = d.getVar("UEFI_SB_AUTH_VARS_DIR")
    signature_list_guid = d.getVar("SIGNATURE_LIST_GUID")

    if os.path.isdir(uefi_sb_auth_vars_dir):
        # No need to regenerate the authenticated variables
        return

    uefi_sb_key_types = ["PK", "KEK", "DB", "DBX"]

    subprocess.check_call(["mkdir", uefi_sb_auth_vars_dir])

    # Check if the cert/key files exists.
    for key_type in uefi_sb_key_types:
        uefi_sb_auth_vars_path = f"{uefi_sb_auth_vars_dir}/{key_type}"

        dbx_str = "DBX/" if key_type == "DBX" else ""
        cert_key_path = f"{uefi_sb_keys_dir}/{dbx_str}{key_type}"

        if (not os.path.isfile(f"{cert_key_path}.crt") or
            not os.path.isfile(f"{cert_key_path}.key")):
            bb.error(f"key/cert files for {key_type} key type are missing.")

        # Generate efi signature list from the certificates created above
        subprocess.check_call(["cert-to-efi-sig-list", "-g", signature_list_guid,
            f"{cert_key_path}.crt", f"{uefi_sb_auth_vars_path}.esl"])

        # Now, sign the signature lists with their corresponding private keys
        cert_key_used_to_sign = ""
        sign_list_type = ""

        if key_type == "PK" or key_type == "KEK":
            cert_key_used_to_sign = "PK"
            sign_list_type = key_type
        else:
            cert_key_used_to_sign = "KEK"
            sign_list_type = key_type.lower()

        subprocess.check_call(["sign-efi-sig-list", "-c",
            f"{uefi_sb_keys_dir}/{cert_key_used_to_sign}.crt", "-k",
            f"{uefi_sb_keys_dir}/{cert_key_used_to_sign}.key", sign_list_type,
            f"{uefi_sb_auth_vars_path}.esl", f"{uefi_sb_auth_vars_path}.auth"])          
}

do_configure[postfuncs] += "generate_auth_vars"

do_deploy:append() {    
    cp -r ${UEFI_SB_AUTH_VARS_DIR} ${DEPLOYDIR}
}

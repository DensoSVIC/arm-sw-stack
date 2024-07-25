#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# FVP RD-Kronos specific U-boot support

# Create and add a boot entry to boot order for the update capsule.
# This lets the system look for the update capsule during boot to
# begin a firmware update.

efidebug boot add -b 1001 boot virtio 0:1 /EFI\BOOT\bootaa64.efi
efidebug boot order 1001

# Enroll UEFI Secure Boot authenticated variables if
# uefi secure boot feature is enabled.

# UEFI_SB_AUTH_VARS_NAME variable will be passed from the
# U-Boot recipe if uefi secure boot feature is enabled.
uefi_sb_auth_vars_name=""

if test -n ${uefi_sb_auth_vars_name}; then
    key_types="PK KEK db dbx"

    for key in ${key_types};
    do
        skip_loop=0
        error_log=""

        if test ${skip_loop} -ne 1; then
            env print -e -n ${key}

            if test $? -eq 0; then
                echo "${key} key has already been enrolled!"
                skip_loop=1
            fi
        fi

        if test ${skip_loop} -ne 1; then
            load virtio 0:1 ${loadaddr} "${uefi_sb_auth_vars_name}/${key}.auth"

            if test $? -ne 0; then
                error_log="Failed to load ${key} key."
                skip_loop=1
            fi
        fi

        if test ${skip_loop} -ne 1; then
            setenv -e -nv -bs -rt -at -i ${loadaddr}:$filesize ${key}

            if test $? -ne 0; then
                error_log="Failed to enroll ${key} key."
                skip_loop=1
            else
                echo "${key} key is enrolled successfully!"
            fi
        fi

        echo "${error_log}"
    done
fi

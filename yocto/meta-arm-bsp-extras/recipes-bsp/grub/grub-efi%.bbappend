#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

UEFI_SECURE_BOOT ?= "0"

require ${@oe.utils.vartrue("UEFI_SECURE_BOOT", "grub-efi-uefi-secure-boot.inc", "", d)}

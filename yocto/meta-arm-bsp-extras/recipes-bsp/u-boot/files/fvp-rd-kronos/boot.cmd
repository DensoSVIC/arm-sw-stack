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

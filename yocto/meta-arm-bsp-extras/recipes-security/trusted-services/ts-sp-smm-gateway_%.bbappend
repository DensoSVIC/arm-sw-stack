#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

require ts-arm-platforms-extras.inc

EXTRA_OECMAKE:append:fvp-rd-kronos = " -DMM_COMM_BUFFER_ADDRESS="0x00000000 0xffbf0000" \
    -DMM_COMM_BUFFER_PAGE_COUNT="1" \
    -DUEFI_AUTH_VAR=ON \
    -DUEFI_INTERNAL_CRYPTO=ON \
    "

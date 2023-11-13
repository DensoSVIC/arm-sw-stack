#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend := "${THISDIR}/files/mbedtls:"
ZEPHYR_PATCHDIR = "zephyr"

ZEPHYR_MBEDTLS_PATCHES ?= "\
    file://0001-mbedtls-Fix-dependencies-with-PSA-crypto.patch;patchdir=${ZEPHYR_PATCHDIR} \
    file://0002-mbedtls-Enable-missing-algorithms-for-psa-arch-tests.patch;patchdir=${ZEPHYR_PATCHDIR} \
"
SRC_URI += "${ZEPHYR_MBEDTLS_PATCHES}"

#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend := "${THISDIR}/files/mbedtls:"
ZEPHYR_PATCHDIR = "zephyr"
ZEPHYR_MBEDTLS_PATCHDIR = "modules/crypto/mbedtls"

ZEPHYR_MBEDTLS_PATCHES ?= "\
    file://0001-mbedtls-Fix-dependencies-with-PSA-crypto.patch;patchdir=${ZEPHYR_PATCHDIR} \
    file://0002-mbedtls-Enable-missing-algorithms-for-psa-arch-tests.patch;patchdir=${ZEPHYR_PATCHDIR} \
    file://0003-crypto-Remove-transparent-key-check-in-psa_asymmetric.patch;patchdir=${ZEPHYR_MBEDTLS_PATCHDIR} \
    file://0004-crypto-Support-psa_destroy_key-in-wrapper.patch;patchdir=${ZEPHYR_MBEDTLS_PATCHDIR} \
    file://0005-Introduce-opaque-rss-driver.patch;patchdir=${ZEPHYR_MBEDTLS_PATCHDIR} \
    file://0006-mbedtls-Add-rss-driver-to-build.patch;patchdir=${ZEPHYR_PATCHDIR} \
"
SRC_URI += "${ZEPHYR_MBEDTLS_PATCHES}"

#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend := "${THISDIR}/files/mbedtls:"
ZEPHYR_MBEDTLS_PATCHDIR = "modules/crypto/mbedtls"

# /* cSpell:disable */
ZEPHYR_MBEDTLS_PATCHES ?= "\
    file://0001-crypto-Remove-transparent-key-check-in-psa_asymmetri.patch;patchdir=${ZEPHYR_MBEDTLS_PATCHDIR} \
    file://0002-crypto-Support-psa_destroy_key-in-wrapper.patch;patchdir=${ZEPHYR_MBEDTLS_PATCHDIR} \
    file://0003-Introduce-opaque-rss-driver.patch;patchdir=${ZEPHYR_MBEDTLS_PATCHDIR} \
"
# /* cSpell:enable */

SRC_URI += "${ZEPHYR_MBEDTLS_PATCHES}"

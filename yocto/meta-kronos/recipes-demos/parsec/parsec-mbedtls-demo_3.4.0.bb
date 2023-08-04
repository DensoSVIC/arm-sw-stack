#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "Parsec demo based on mbedtls"
DESCRIPTION = "mbedtls is a lean open source crypto library for      \
               providing SSL and TLS support in your programs. The   \
               demonstration shows the usage of PARSEC in a TLS      \
               handshake process. It is consist of a TLS server and  \
               a TLS client."

HOMEPAGE = "https://tls.mbed.org/"

LICENSE = "Apache-2.0"

LIC_FILES_CHKSUM = "file://LICENSE;md5=3b83ef96387f14655fc854ddc3c6bd57"

DEPENDS = "parsec-se-driver python3-jinja2-native python3-jsonschema-native"

PV = "3.4.0+git${SRCPV}"

SRC_URI = "git://github.com/Mbed-TLS/mbedtls.git;protocol=https;branch=development \
           file://0001-Define-key-location-for-secure-element.patch \
           file://0002-Invoke-SE-driver-in-verifying-X509-certificate.patch \
           file://0003-program-ssl-Modify-ssl_client1.c-to-register-parsec-.patch \
           file://0004-program-ssl-Make-ssl_client1.c-accept-server-IP-addr.patch \
           file://0005-program-Link-to-parsec_se_driver.patch \
           "

SRCREV = "63a21f4cda8b3d21fe5f85e6efc62d924b492757"
S = "${WORKDIR}/git"

inherit cmake python3native

set_config() {
    ${S}/scripts/config.py set MBEDTLS_USE_PSA_CRYPTO
    ${S}/scripts/config.py set MBEDTLS_PSA_CRYPTO_SE_C
}
set_config[doc] = 'Use the mbedtls configuration script to modify the header \
                   file to enable the PSA Crypto Secure Element interfaces'

do_configure[prefuncs] += "set_config"

# nooelint: oelint.vars.insaneskip
INSANE_SKIP:${PN}-dbg += "buildpaths"

OECMAKE_TARGET_COMPILE = "ssl_client1 ssl_server"

do_install () {
    install -d ${D}${bindir}
    install -m 0755 "${B}/programs/ssl/ssl_client1" "${D}${bindir}"
    install -m 0755 "${B}/programs/ssl/ssl_server" "${D}${bindir}"
}

#
# Based on: https://github.com/PelionIoT/meta-edge/blob/5b8705c23cfc2ddfa0a7596efdae3546bd49d647/recipes-edge/parsec-se-driver/parsec-se-driver.bb
# In open-source project: meta-edge
# Original file: SPDX-FileCopyrightText: <text>Copyright (c) 2023
# Izuma and affiliates</text>
# Modifications: SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited
# and/or its affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# Changes:
# 1) Include parsec-se-driver-crates.inc instead of parsec-se-driver.inc
# 2) Remove mbedtls from SRC_URI and add it to DEPENDS
# 3) Export MBEDTLS_INCLUDE_DIR directly in the recipe
#

SUMMARY = "Parsec Secure Element Driver"
DESCRIPTION = "An implementation of a PSA Secure Element using the Parsec \
               service, compiling into a library"
HOMEPAGE = "https://github.com/parallaxsecond/parsec-se-driver"
LICENSE = "Apache-2.0"

DEPENDS = "mbedtls"

PV = "${PARSEC_version}+git${SRCPV}"

SRC_URI = "git://github.com/parallaxsecond/parsec-se-driver.git;protocol=https;branch=main"
SRCREV = "08572372c8fecb90eb4318548ee03855956237ba"
S = "${WORKDIR}/git"

inherit cargo

PARSEC_version = "0.6.0"

export MBEDTLS_INCLUDE_DIR = "${STAGING_INCDIR}"

TOOLCHAIN = "clang"

# nooelint: oelint.vars.insaneskip
INSANE_SKIP:${PN}-staticdev += "buildpaths"

do_install() {
    install -d "${D}/${libdir}"
    install -m 755 "${B}/target/${RUST_TARGET_SYS}/release/libparsec_se_driver.a" "${D}/${libdir}"
}

include parsec-se-driver-crates.inc

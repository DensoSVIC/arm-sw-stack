#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend := "${THISDIR}/files/:"

# /* cSpell:disable */
SRC_URI_EXTRA_PATCHES = "\
    file://0001-Cargo.toml-Update-parsec-client-and-psa-crypto-depen.patch \
    file://0002-.gitignore-Start-tracking-Cargo.lock-file.patch \
    file://0003-ci.sh-Update-parsec-version-to-1.3.0.patch \
    file://0004-ci.yml-Run-update-package-list-before-installing-pac.patch \
    file://0005-ci-config.toml-Add-Parsec-socket-path.patch \
    "
# /* cSpell:enable */

SRC_URI:append = " ${SRC_URI_EXTRA_PATCHES}"

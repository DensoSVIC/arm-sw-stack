#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend := "${THISDIR}/files/:"

SRC_URI += " \
    crate://crates.io/base64/0.21.2;name=base64-0.21.2 \
    crate://crates.io/picky-asn1/0.7.2;name=picky-asn1-0.7.2 \
    crate://crates.io/picky-asn1-der/0.4.0;name=picky-asn1-der-0.4.0 \
    crate://crates.io/picky-asn1-x509/0.11.0;name=picky-asn1-x509-0.11.0 \
    file://0001-Bump-ASN1-crates-dependencies.patch \
"

SRC_URI[base64-0.21.2.sha256sum] = "604178f6c5c21f02dc555784810edfb88d34ac2c73b2eae109655649ee73ce3d"
SRC_URI[picky-asn1-0.7.2.sha256sum] = "6f338f1fd4f3e13e75e986ca29f2a3c62528d88d3cbadf4afdcefb6b087f2d32"
SRC_URI[picky-asn1-der-0.4.0.sha256sum] = "e47267a46f4ea246b772381970b8ed3f15963dd3e15ffc2c3f4ac3bc2d77384b"
SRC_URI[picky-asn1-x509-0.11.0.sha256sum] = "25c2718a5fe2b6c4b651bad5514699182f01db1b2f256fd6ab237f5879a01a4a"

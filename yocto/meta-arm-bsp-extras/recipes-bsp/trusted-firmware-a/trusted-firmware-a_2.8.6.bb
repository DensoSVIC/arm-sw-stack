#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

require recipes-bsp/trusted-firmware-a/trusted-firmware-a.inc

LICENSE = "BSD-3-Clause & MIT"
LIC_FILES_CHKSUM += "file://docs/license.rst;md5=b2c740efedc159745b9b31f88ff03dde"
# nooelint: oelint.vars.mispell
LIC_FILES_CHKSUM_MBEDTLS = "file://mbedtls/LICENSE;md5=3b83ef96387f14655fc854ddc3c6bd57"

SRC_URI += "file://rwx-segments.patch"

# TF-A v2.8.6
SRCREV_tfa = "ff0bd5f9bb2ba2f31fb9cec96df917747af9e92d"
# nooelint: oelint.vars.mispell
SRCBRANCH = "lts-v2.8"

# mbed TLS v2.28.2
SRC_URI_MBEDTLS = "git://github.com/ARMmbed/mbedtls.git;name=mbedtls;protocol=https;destsuffix=git/mbedtls;branch=mbedtls-2.28"
SRCREV_mbedtls = "89f040a5c938985c5f30728baed21e49d0846a53"

#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

COMPATIBLE_MACHINE:fvp-rd-kronos = "fvp-rd-kronos"

SRCREV_trusted-services:fvp-rd-kronos = "08b3d39471f4914186bd23793dc920e83b0e3197"
SRCREV_mbedtls:fvp-rd-kronos = "8c89224991adff88d53cd380f42a2baa36f91454"
SRCREV_nanopb:fvp-rd-kronos = "df0e92f474f9cca704fe2b31483f0b4d1b1715a4"

# nooelint: oelint.append.protvars.LIC_FILES_CHKSUM
LIC_FILES_CHKSUM:remove:fvp-rd-kronos = "file://../mbedtls/LICENSE;md5=379d5819937a6c2f1ef1630d341e026d"
# nooelint: oelint.append.protvars.LIC_FILES_CHKSUM
LIC_FILES_CHKSUM:append:fvp-rd-kronos = " file://../mbedtls/LICENSE;md5=3b83ef96387f14655fc854ddc3c6bd57"

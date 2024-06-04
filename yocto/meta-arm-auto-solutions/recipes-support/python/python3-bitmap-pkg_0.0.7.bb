#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "BitMap for python"
DESCRIPTION = "This package provides a BitMap class which is an array of bits \
               stored in compact format."
HOMEPAGE = "https://github.com/wanji/bitmap"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://PKG-INFO;md5=d3ed195d25ba27a7252888203ecab7d6"
PYPI_PACKAGE = "bitmap"

SRC_URI[sha256sum] = "e7fb8fcdade2223348132c108619f93178b563f584ed552aa5d3c6f946958a2d"

inherit pkgconfig pypi setuptools3

BBCLASSEXTEND = "native nativesdk"

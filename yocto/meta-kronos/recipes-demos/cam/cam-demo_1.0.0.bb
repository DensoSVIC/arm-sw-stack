#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "CAM demo"
DESCRIPTION = "The Critical Application Monitoring (CAM) project implements a  \
               solution for monitoring applications using a service running on \
               a higher safety level system."
HOMEPAGE = "https://cam.docs.arm.com/"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://license.rst;md5=214c73aa30e7f71d6173261fe950f51b"


BRANCH_CAM ?= "branch=main"
SRCREV_CAM ?= "fe20aa770f6191462b31b00de82d0640006d6802"

RDEPENDS:${PN} += "python3 \
                   python3-wheel \
                   python3-pyyaml \
                   python3-future \
                   python3-bitmap-pkg \
"

PV .= "+git${SRCPV}"
SRC_URI = "${SRC_URI_CAM};${BRANCH_CAM}"
SRCREV = "${SRCREV_CAM}"
S = "${WORKDIR}/git"

inherit cmake pkgconfig python_setuptools_build_meta

PEP517_SOURCE_PATH = "${S}/cam-tool"

EXTRA_OECMAKE:append = "\
    -DBUILD_TESTING=OFF \
    -DBUILD_DOCUMENTATION=OFF \
"

do_configure() {
    cmake_do_configure
    python_pep517_do_configure
}

do_compile() {
    cmake_do_compile
    python_pep517_do_compile
}

do_install() {
    cmake_do_install
    python_pep517_do_install
}

FILES:${PN} += "${datadir}/*"

#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

require actuation-common.inc

DEPENDS += "\
    ament-cmake-auto-native \
    ament-cmake-python-native \
    ament-cmake-target-dependencies-native \
    autoware-cmake-native \
"

PV .= "+git${SRCPV}"
SRC_URI = "${SRC_URI_ACTUATION};${BRANCH_ACTUATION}"
SRCREV = "${SRCREV_ACTUATION}"
S = "${WORKDIR}/git"

inherit python3native pkgconfig cmake

EXTRA_OECMAKE:append = "\
    -DBUILD_TESTING=OFF \
    -DROS_DISTRO=galactic \
"

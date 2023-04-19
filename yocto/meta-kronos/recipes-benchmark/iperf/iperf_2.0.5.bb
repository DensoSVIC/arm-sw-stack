#
# Based on: http://cgit.openembedded.org/meta-openembedded/tree/meta-oe/recipes-benchmark/iperf/iperf_2.0.5.bb?id=ea5310a1556d215f7cce0a326d518512c160f1ad
# In open-source project: meta-openembedded
#
# Original file: SPDX-FileCopyrightText: <text>Copyright 2018 meta-openembedded
# community</text>
# Modifications: SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or
# its affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# Changes:
# 1) Add CVE_PRODUCT = "iperf_project:iperf"
# 2) Add 0003-iperf-backport-changes-from-2.0.13.patch
# 3) Add BBCLASSEXTEND += "native"
#
DESCRIPTION = "Iperf is a tool to measure maximum TCP bandwidth, allowing the tuning of various parameters and UDP characteristics"
HOMEPAGE = "http://dast.nlanr.net/Projects/Iperf/"
SECTION = "console/network"
LICENSE = "BSD-2-Clause"
LIC_FILES_CHKSUM = "file://COPYING;md5=e8478eae9f479e39bc34975193360298"

SRC_URI = " ${SOURCEFORGE_MIRROR}/iperf/${BP}.tar.gz \
            file://0001-fix-out-of-tree-config.patch \
            file://0002-fix-bool-size-m4.patch \
            file://0001-headers.h-remove-math.h-include-that-breaks-aarch64.patch \
            file://0003-iperf-backport-changes-from-2.0.13.patch \
            file://0004-iperf-2.0.5_ManPage.patch \
          "

SRC_URI[md5sum] = "44b5536b67719f4250faed632a3cd016"
SRC_URI[sha256sum] = "636b4eff0431cea80667ea85a67ce4c68698760a9837e1e9d13096d20362265b"

S = "${WORKDIR}/${BP}"

inherit autotools pkgconfig

EXTRA_OECONF = "--exec-prefix=${STAGING_DIR_HOST}${layout_exec_prefix}"

#PACKAGECONFIG ??= "${@bb.utils.contains('DISTRO_FEATURES', 'ipv6', 'ipv6', '', d)}"
PACKAGECONFIG[ipv6] = "--enable-ipv6,--disable-ipv6,"

CVE_PRODUCT = "iperf_project:iperf"

BBCLASSEXTEND += "native"

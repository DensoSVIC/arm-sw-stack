#
# SPDX-FileCopyrightText: <text>Copyright 2026 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT
#
# Bump pseudo to 1.9.11 to pick up the openat2() wrapper.
#
# Without it, builds fail on hosts whose GNU tar uses openat2() (tar >= 1.35,
# e.g. Ubuntu 24.04). tar calls openat2(dirfd, "<name>/", ...) to open nested
# directories relative to an O_PATH directory fd. pseudo 1.9.0 does not
# intercept openat2(), so the fd tar obtains is never registered in pseudo's
# fd path table. A following mkdirat(dirfd, "<name>") then hits
# base_path()/fd_path() with an unknown fd, pseudo rewrites the path to NULL
# and the real syscall returns EFAULT:
#
#   got *at() syscall for unknown directory, fd N
#   unknown base path for fd N, path include
#   couldn't allocate absolute path for 'include'.
#   tar: ./usr/include: Cannot mkdir: Bad address
#
# This broke perform_packagecopy() (tar | tar) for most recipes, e.g.
# linux-libc-headers do_package.
#
# The base SRC_URI is replaced to drop the two 1.9.0-era patches that no longer
# apply to 1.9.11: 0001-configure-Prune-PIE-flags.patch (now done in configure)
# and glibc238.patch (fixed upstream). The class-native/class-nativesdk
# SRC_URI:append entries in pseudo_git.bb are left untouched, so
# older-glibc-symbols.patch is still referenced once for native/nativesdk.
#
# FILESEXTRAPATHS makes this layer's rebased older-glibc-symbols.patch take
# precedence over the poky 1.9.0 copy.

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI = "git://git.yoctoproject.org/pseudo;branch=master;protocol=https \
           file://fallback-passwd \
           file://fallback-group \
           "

SRCREV = "ba8887e5f1e922f866681ec7dec1a00b602a9328"
PV = "1.9.11"

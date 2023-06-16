#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend := "${THISDIR}:${THISDIR}/files:"

SRC_URI_EXTRA_PATCHES = "\
    file://0001-rpmsg-virtio-Make-size-and-number-of-buffers-configu.patch \
    file://0002-mailbox-Make-transmit-queue-size-configurable.patch \
    file://0003-virtio-disable-remoteproc-virtio-RPMSG-to-use-DMA-ap.patch \
    file://0004-mailbox-Patch-MHUv2-driver-to-make-it-work-with-MHUv.patch \
    "
SRC_URI += "${SRC_URI_EXTRA_PATCHES}"
SRC_URI:append = " file://kronos-kmeta-extra;type=kmeta;name=kronos-kmeta-extra;destsuffix=kronos-kmeta-extra"

KERNEL_FEATURES:append = "\
    features/arm-mhuv2/arm-mhuv2.scc \
    features/mailbox/mailbox.scc \
    features/remoteproc/remoteproc.scc \
    features/rpmsg-virtio/rpmsg-virtio.scc \
    features/vswitch/vswitch.scc \
    features/xen/xen.scc \
    "

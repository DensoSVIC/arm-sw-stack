#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

PREFERRED_VERSION_linux-yocto = "6.1%"
PREFERRED_VERSION_linux-yocto-rt = "6.1%"

FILESEXTRAPATHS:prepend := "${THISDIR}:${THISDIR}/files:"

SRC_URI_EXTRA_PATCHES = "\
    file://0001-rpmsg-virtio-Make-size-and-number-of-buffers-configu.patch \
    file://0002-virtio-disable-remoteproc-virtio-RPMSG-to-use-DMA-ap.patch \
    file://0003-dt-bindings-mailbox-arm-mhuv3-Add-bindings.patch \
    file://0004-mailbox-arm-mhuv3-Add-driver.patch \
    file://0005-xen-netback-add-software-timestamp-capabilities.patch \
    "
SRC_URI += "${SRC_URI_EXTRA_PATCHES}"
SRC_URI:append = " file://arm-auto-solutions-kmeta-extra;type=kmeta;name=arm-auto-solutions-kmeta-extra;destsuffix=arm-auto-solutions-kmeta-extra"

KERNEL_FEATURES:append = "\
    features/arm-mhuv3/arm-mhuv3.scc \
    features/bpf/bpf.scc \
    features/mailbox/mailbox.scc \
    features/remoteproc/remoteproc.scc \
    features/rpmsg-virtio/rpmsg-virtio.scc \
    features/vswitch/vswitch.scc \
    features/xen/xen.scc \
    features/arm-pci-ahci/arm-pci-ahci.scc \
    features/cfs-bandwidth/cfs-bandwidth.scc \
    "

# Currently, FFA is not supported in Xen, disable it
KERNEL_FEATURES:append:virtualization = " features/ffa-tee/disable-ffa-tee.scc"
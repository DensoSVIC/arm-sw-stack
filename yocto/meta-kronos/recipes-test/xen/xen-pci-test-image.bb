# nooelint: oelint.var.mandatoryvar - This recipe has no source files
#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "Xen PCI Passthrough disk image"
DESCRIPTION = "Deploy an empty disk image that can be used for Xen PCI Passthrough"
HOMEPAGE = "https://kronos-ref-stack.docs.arm.com/"
LICENSE = "MIT"

inherit deploy

XEN_PCI_IMAGE_NAME = "xen-pci-test-image"
# Size of disk in MB
XEN_PCI_IMAGE_SIZE ?= "16"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_image() {
    dd if=/dev/zero of=${WORKDIR}/${XEN_PCI_IMAGE_NAME}.wic \
       bs=1M count=${XEN_PCI_IMAGE_SIZE} status=none
}

do_deploy() {
    # Deploy the target disk image to the deploy folder
    install -m 644 ${WORKDIR}/${XEN_PCI_IMAGE_NAME}.wic ${DEPLOYDIR}
}

addtask image before do_deploy
addtask deploy after do_image

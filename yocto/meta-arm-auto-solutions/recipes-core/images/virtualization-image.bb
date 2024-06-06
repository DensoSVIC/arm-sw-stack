#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "Virtualization image"
DESCRIPTION = "An image recipe, based on core-image, which additionally \
includes xen and xen-cfg in the boot partition"
LICENSE = "MIT"

IMAGE_INSTALL = ""
IMAGE_LINGUAS = ""

inherit core-image

IMAGE_OVERHEAD_FACTOR = "1.5"

inherit features_check
REQUIRED_IMAGE_FEATURES = "virtualization"
CONFLICT_IMAGE_FEATURES = "baremetal domu"

GRUB_CFG_FILE = "virtualization-grub.cfg"

IMAGE_EFI_BOOT_FILES:append = " xen-${MACHINE}.efi;xen.efi xen.cfg"
do_image_wic[depends] += "xen:do_deploy xen-cfg:do_deploy "

EXTRA_IMAGEDEPENDS += "xen xen-cfg"

update_si_bridges() {
    sed -i -r 's/192.168.0.2/192.168.0.253/g' ${IMAGE_ROOTFS}/etc/systemd/network/10-brsi0.network
    sed -i -r 's/192.168.1.2/192.168.1.253/g' ${IMAGE_ROOTFS}/etc/systemd/network/10-brsi1.network
    sed -i -r 's/192.168.2.2/192.168.2.253/g' ${IMAGE_ROOTFS}/etc/systemd/network/10-brsi2.network
}

update_si_bridges[doc] = "Sets the Safety Island Bridges IP address for Dom0."

ROOTFS_POSTPROCESS_COMMAND += "update_si_bridges; "

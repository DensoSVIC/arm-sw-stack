#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "Virtualization image"
DESCRIPTION = "An image recipe, based on core-image, which additionally \
includes xen and xen-cfg in the boot partition"

IMAGE_INSTALL = ""
IMAGE_LINGUAS = ""

inherit core-image

IMAGE_OVERHEAD_FACTOR = "1.5"

inherit features_check
REQUIRED_IMAGE_FEATURES = "virtualization"
CONFLICT_IMAGE_FEATURES = "baremetal domu"
COMPATIBLE_MACHINE = "fvp-rd-kronos"

GRUB_CFG_FILE = \
"${KRONOS_VIRTUALIZATION_DYNAMIC_DIR}/wic/virtualization-grub.cfg"

IMAGE_EFI_BOOT_FILES:append = " xen-${MACHINE}.efi;xen.efi xen.cfg"
do_image_wic[depends] += "xen:do_deploy xen-cfg:do_deploy "

EXTRA_IMAGEDEPENDS += "xen xen-cfg"


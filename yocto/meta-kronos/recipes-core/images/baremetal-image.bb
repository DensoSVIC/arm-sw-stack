#
# Copyright (c) 2023, Arm Limited.
#
# SPDX-License-Identifier: MIT

SUMMARY = "Baremetal image"
DESCRIPTION = "An image recipe, based on core-image"
COMPATIBLE_MACHINE = "fvp-rd-kronos"

inherit core-image

IMAGE_INSTALL = "\
    packagegroup-core-boot ${CORE_IMAGE_EXTRA_INSTALL} \
    packagegroup-machine-base \
    packagegroup-core-ssh-openssh \
"
IMAGE_LINGUAS = ""

GRUB_LINUX_APPEND:append = " maxcpus=4"

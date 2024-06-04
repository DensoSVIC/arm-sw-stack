#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "Xen domu1 image"
DESCRIPTION = "An image recipe, based on core-image, which builds a qcow2 disk\
image for a DomU without a boot partition"
LICENSE = "MIT"

DOMU_HOSTNAME = "domu1"

require domu-image.inc

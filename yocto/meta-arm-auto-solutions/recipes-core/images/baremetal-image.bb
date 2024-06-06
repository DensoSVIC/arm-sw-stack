#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "Baremetal image"
DESCRIPTION = "An image recipe, based on core-image"
LICENSE = "MIT"

IMAGE_INSTALL = ""
IMAGE_LINGUAS = ""

inherit core-image

IMAGE_OVERHEAD_FACTOR = "1.5"

inherit features_check
REQUIRED_IMAGE_FEATURES = "baremetal"
CONFLICT_IMAGE_FEATURES = "virtualization domu"

BAREMETAL_IMAGE_NUM_CPUS ?= "4"
# The total RAM size is 2G and 32M of it has been allocated to OP-TEE. The
# remaining RAM space size is (2048 - 32)M
BAREMETAL_IMAGE_MEM_SIZE ?= "2016M"
GRUB_LINUX_APPEND:append = "\
    maxcpus=${BAREMETAL_IMAGE_NUM_CPUS} \
    mem=${BAREMETAL_IMAGE_MEM_SIZE} \
    "
TEST_CPU_HOTPLUG_NUM_CPUS = "${BAREMETAL_IMAGE_NUM_CPUS}"

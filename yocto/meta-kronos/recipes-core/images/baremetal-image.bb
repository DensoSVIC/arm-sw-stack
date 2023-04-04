#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
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

BAREMETAL_IMAGE_NUM_CPUS ?= "4"
BAREMETAL_IMAGE_MEM_SIZE ?= "2G"
GRUB_LINUX_APPEND:append = "\
    maxcpus=${BAREMETAL_IMAGE_NUM_CPUS} \
    mem=${BAREMETAL_IMAGE_MEM_SIZE} \
    "
TEST_SMP_NUM_CPUS = "${BAREMETAL_IMAGE_NUM_CPUS}"

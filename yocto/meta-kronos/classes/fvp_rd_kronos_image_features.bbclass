#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# This bbclass handles, via the EXTRA_IMAGE_FEATURES variable, the following
# features that will be used to select packages to be installed on rootfs,
# Safety Island image and integration testing.
IMAGE_FEATURES[validitems] += " \
    baremetal \
    virtualization \
    domu \
    "

DOMU_INSTANCES ?= "2"

IMAGE_FEATURES_CONFLICTS_baremetal = "virtualization domu"
IMAGE_FEATURES_CONFLICTS_virtualization = "baremetal domu"
IMAGE_FEATURES_CONFLICTS_domu = "baremetal virtualization"


FEATURE_PACKAGES_COMMON = " \
    packagegroup-core-boot \
    packagegroup-core-ssh-openssh \
    packagegroup-machine-base \
    ${CORE_IMAGE_EXTRA_INSTALL} \
    "

FEATURE_PACKAGES_baremetal = "${FEATURE_PACKAGES_COMMON}"

FEATURE_PACKAGES_virtualization = " \
    ${FEATURE_PACKAGES_COMMON} \
    domu-package \
    kernel-module-xen-gntalloc \
    kernel-module-xen-gntdev \
    kernel-module-xen-netback \
    xen-tools \
    "

FEATURE_PACKAGES_domu = " \
    packagegroup-core-boot \
    ${CORE_IMAGE_EXTRA_INSTALL} \
    "

require ${@bb.utils.contains('MACHINE', 'fvp-rd-kronos', bb.utils.contains('EXTRA_IMAGE_FEATURES', 'virtualization', "${KRONOS_VIRTUALIZATION_DYNAMIC_DIR}/conf/machine/include/fvp-rd-kronos-xen.inc", '', d), '', d)}

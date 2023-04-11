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
    packagegroup-security-parsec \
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
    virtualization-integration-tests-ptest \
    "

FEATURE_PACKAGES_domu = " \
    packagegroup-core-boot \
    ${CORE_IMAGE_EXTRA_INSTALL} \
    "

require ${@bb.utils.contains('MACHINE', 'fvp-rd-kronos', 'conf/machine/include/fvp-rd-kronos-extras.inc', '', d)}

def add_extra_test_suites(d):
    test_suites = ""
    extra_img_feat = (d.getVar('EXTRA_IMAGE_FEATURES') or "")
    for feature in extra_img_feat.split():
        if feature == 'virtualization':
            test_suites += ' test_40_virtualization'

    return test_suites

TEST_SUITES:append = " test_10_linuxlogin ${@add_extra_test_suites(d)}"

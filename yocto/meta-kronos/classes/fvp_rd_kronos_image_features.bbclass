#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# Add an override so that variables can have a value set only if 'feature' is
# enabled in EXTRA_IMAGE_FEATURES:
# VAR is "val" only if 'hipc-validation' is in EXTRA_IMAGE_FEATURES
# e.g. VAR:hipc-validation = "val"
OVERRIDES:append = "${@bb.utils.contains('EXTRA_IMAGE_FEATURES', \
                    'hipc-validation', ':hipc-validation', '', d)}"

# This bbclass handles, via the EXTRA_IMAGE_FEATURES variable, the following
# features that will be used to select packages to be installed on rootfs,
# Safety Island image and integration testing.
IMAGE_FEATURES[validitems] += " \
    baremetal \
    hipc-validation \
    virtualization \
    domu \
    "

DOMU_INSTANCES ?= "2"

IMAGE_FEATURES_CONFLICTS_baremetal = "virtualization domu"
IMAGE_FEATURES_CONFLICTS_virtualization = "baremetal domu"
IMAGE_FEATURES_CONFLICTS_domu = "baremetal virtualization"


FEATURE_PACKAGES_COMMON = " \
    arm-si-rproc-mod \
    packagegroup-core-boot \
    packagegroup-core-ssh-openssh \
    packagegroup-machine-base \
    packagegroup-security-parsec \
    rpmsg-net-mod \
    ${CORE_IMAGE_EXTRA_INSTALL} \
    "

FEATURE_PACKAGES_baremetal = " \
    ${FEATURE_PACKAGES_COMMON} \
    docker-ce \
    "

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
    docker-ce \
    ${CORE_IMAGE_EXTRA_INSTALL} \
    "

require ${@bb.utils.contains('MACHINE', 'fvp-rd-kronos', 'conf/machine/include/fvp-rd-kronos-extras.inc', '', d)}

ZEPHYR_APP_SAFETY_ISLAND_CL0:hipc-validation = "zperf"
ZEPHYR_APP_SAFETY_ISLAND_CL1:hipc-validation = "zperf"
ZEPHYR_APP_SAFETY_ISLAND_CL2:hipc-validation = "zperf"

def add_extra_test_suites(d):
    test_suites = ""
    extra_img_feat = (d.getVar('EXTRA_IMAGE_FEATURES') or "")
    for feature in extra_img_feat.split():
        if feature == 'virtualization':
            test_suites += ' test_40_virtualization'

    return test_suites

TEST_SUITES:append = " test_10_linuxlogin \
    ${@add_extra_test_suites(d)} \
    test_40_parsec \
"

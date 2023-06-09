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
                    'baremetal', ':baremetal', '', d)}"

OVERRIDES:append = "${@bb.utils.contains('EXTRA_IMAGE_FEATURES', \
                    'domu', ':domu', '', d)}"

OVERRIDES:append = "${@bb.utils.contains('EXTRA_IMAGE_FEATURES', \
                    'hipc-validation', ':hipc-validation', '', d)}"

OVERRIDES:append = "${@bb.utils.contains('EXTRA_IMAGE_FEATURES', \
                    'virtualization', ':virtualization', '', d)}"

OVERRIDES:append = "${@bb.utils.contains('EXTRA_IMAGE_FEATURES', \
                    'actuation', ':actuation', '', d)}"

OVERRIDES:append = "${@bb.utils.contains('EXTRA_IMAGE_FEATURES', \
                    'si0-ethernet0', ':si0-ethernet0', '', d)}"

# This bbclass handles, via the EXTRA_IMAGE_FEATURES variable, the following
# features that will be used to select packages to be installed on rootfs,
# Safety Island image and integration testing.
IMAGE_FEATURES[validitems] += " \
    baremetal \
    hipc-validation \
    virtualization \
    domu \
    actuation \
    si0-ethernet0 \
    "

DOMU_INSTANCES ?= "2"

IMAGE_FEATURES_CONFLICTS_baremetal = "virtualization domu"
IMAGE_FEATURES_CONFLICTS_virtualization = "baremetal domu"
IMAGE_FEATURES_CONFLICTS_domu = "baremetal virtualization"
IMAGE_FEATURES_CONFLICTS_hipc-validation = "si0-ethernet0 actuation"
IMAGE_FEATURES_CONFLICTS_actuation = "si0-ethernet0 hipc-validation"
IMAGE_FEATURES_CONFLICTS_si0-ethernet0 = "hipc-validation actuation"

FEATURE_PACKAGES_COMMON = " \
    arm-si-rproc-mod \
    kronos-network-conf \
    packagegroup-core-boot \
    packagegroup-core-ssh-openssh \
    packagegroup-machine-base \
    packagegroup-security-parsec \
    rpmsg-net-mod \
    systemd-conf-kronos \
    ${CORE_IMAGE_EXTRA_INSTALL} \
    "

FEATURE_PACKAGES_baremetal = " \
    ${FEATURE_PACKAGES_COMMON} \
    podman \
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
    kronos-network-conf \
    packagegroup-core-boot \
    systemd-conf-kronos \
    podman \
    ${CORE_IMAGE_EXTRA_INSTALL} \
    "

ACTUATION_PACKAGES ?= "actuation-player"
FEATURE_PACKAGES_actuation = "${ACTUATION_PACKAGES}"
FEATURE_PACKAGES_actuation:virtualization = ""

EXTRA_IMAGEDEPENDS:append:actuation = " packet-analyzer-native"

FEATURE_PACKAGES_hipc-validation = "iperf"
FEATURE_PACKAGES_hipc-validation:virtualization = ""

require ${@bb.utils.contains('MACHINE', 'fvp-rd-kronos', 'conf/machine/include/fvp-rd-kronos-extras.inc', '', d)}

ZEPHYR_APP_SAFETY_ISLAND_CL0:hipc-validation = "zperf"
ZEPHYR_APP_SAFETY_ISLAND_CL0:si0-ethernet0 = "zperf"
ZEPHYR_APP_SAFETY_ISLAND_CL0:actuation = "actuation"
ZEPHYR_APP_SAFETY_ISLAND_CL1:hipc-validation = "zperf"
ZEPHYR_APP_SAFETY_ISLAND_CL2:hipc-validation = "zperf"

TEST_SUITES_EXTRA ?= " \
    test_10_safety_island_c0 \
    test_10_safety_island_c1 \
    test_10_safety_island_c2 \
    "

TEST_SUITES_EXTRA:hipc-validation = " test_30_hipc"

TEST_SUITES_EXTRA:actuation = " \
    test_30_actuation \
    test_10_safety_island_c1 \
    test_10_safety_island_c2 \
    "

TEST_SUITES_EXTRA:hipc-validation:virtualization = " \
    test_30_hipc_virtualization \
    "

TEST_SUITES_EXTRA:si0-ethernet0 = " \
    test_30_si0_ethernet0 \
    test_10_safety_island_c1 \
    test_10_safety_island_c2 \
    "

TEST_SUITES_EXTRA:append:virtualization = " \
    test_40_virtualization \
    "

TEST_SUITES:append = " \
    test_10_linuxlogin \
    test_40_parsec \
    ${TEST_SUITES_EXTRA} \
"

TEST_SUITES:remove:si0-ethernet0 = "\
    test_00_lcp \
    test_00_trusted_firmware_a \
    test_10_linuxboot \
    test_20_bsp \
    test_10_linuxlogin \
    test_40_parsec \
    "

TEST_SUITES:remove:hipc-validation = " \
    test_20_bsp \
    test_40_parsec \
    "

TEST_SUITES:remove:hipc-validation:virtualization = " \
    test_40_virtualization \
    "

EXTRA_TESTIMAGE_RDEPENDS ?= ""
EXTRA_TESTIMAGE_RDEPENDS:si0-ethernet0 = "iperf-native:do_populate_sysroot"

do_testimage[rdepends] += "${EXTRA_TESTIMAGE_RDEPENDS}"

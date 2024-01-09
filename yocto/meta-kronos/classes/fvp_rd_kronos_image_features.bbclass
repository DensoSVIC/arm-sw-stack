#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
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
                    'cam', ':cam', '', d)}"

OVERRIDES:append = "${@bb.utils.contains('EXTRA_IMAGE_FEATURES', \
                    'si0-bridge-ethernet0', ':si0-bridge-ethernet0', '', d)}"

OVERRIDES:append = "${@bb.utils.contains('EXTRA_IMAGE_FEATURES', \
                    'si-psa-storage-tests', ':si-psa-storage-tests', '', d)}"

OVERRIDES:append = "${@bb.utils.contains('EXTRA_IMAGE_FEATURES', \
                    'si-psa-crypto-tests', ':si-psa-crypto-tests', '', d)}"

# This bbclass handles, via the EXTRA_IMAGE_FEATURES variable, the following
# features that will be used to select packages to be installed on rootfs,
# Safety Island image and integration testing.
IMAGE_FEATURES[validitems] += " \
    baremetal \
    hipc-validation \
    virtualization \
    domu \
    actuation \
    si0-bridge-ethernet0 \
    cam \
    si-psa-storage-tests \
    si-psa-crypto-tests \
    "

DOMU_INSTANCES ?= "2"

IMAGE_FEATURES_CONFLICTS_baremetal = "virtualization domu"
IMAGE_FEATURES_CONFLICTS_virtualization = "baremetal domu"
IMAGE_FEATURES_CONFLICTS_domu = "baremetal virtualization"
IMAGE_FEATURES_CONFLICTS_hipc-validation = "si0-bridge-ethernet0 actuation cam si-psa-storage-tests si-psa-crypto-tests"
IMAGE_FEATURES_CONFLICTS_actuation = "si0-bridge-ethernet0 hipc-validation cam si-psa-storage-tests si-psa-crypto-tests"
IMAGE_FEATURES_CONFLICTS_si0-bridge-ethernet0 = "hipc-validation actuation cam si-psa-storage-tests si-psa-crypto-tests"
IMAGE_FEATURES_CONFLICTS_cam = \
    "si0-bridge-ethernet0 hipc-validation actuation si-psa-storage-tests si-psa-crypto-tests"
IMAGE_FEATURES_CONFLICTS_si-psa-storage-tests = "hipc-validation actuation si0-bridge-ethernet0 cam si-psa-crypto-tests"
IMAGE_FEATURES_CONFLICTS_si-psa-crypto-tests = "hipc-validation actuation si0-bridge-ethernet0 cam si-psa-storage-tests"

FEATURE_PACKAGES_COMMON = " \
    arm-si-rproc-mod \
    iptables \
    kernel-module-openvswitch \
    kronos-network-conf \
    openvswitch \
    packagegroup-core-boot \
    packagegroup-core-ssh-openssh \
    packagegroup-machine-base \
    packagegroup-security-parsec \
    rpmsg-net-mod \
    systemd-conf-kronos \
    systemd-ovs-kronos \
    ${CORE_IMAGE_EXTRA_INSTALL} \
    "

FEATURE_PACKAGES_baremetal = " \
    ${FEATURE_PACKAGES_COMMON} \
    kernel-module-bridge \
    kernel-module-br-netfilter \
    packagegroup-ts-tests-psa \
    parsec-mbedtls-demo \
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

CAM_PACKAGES ?= "cam-app-example cam-service cam-tool linuxptp"
FEATURE_PACKAGES_cam = "${CAM_PACKAGES}"
FEATURE_PACKAGES_cam:virtualization = "linuxptp"

KRONOS_EXTRA_IMAGEDEPENDS = ""
KRONOS_EXTRA_IMAGEDEPENDS:actuation = "packet-analyzer-native:do_addto_recipe_sysroot"

EXTRA_IMAGEDEPENDS:append:baremetal = " ${KRONOS_EXTRA_IMAGEDEPENDS}"
EXTRA_IMAGEDEPENDS:append:virtualization = " ${KRONOS_EXTRA_IMAGEDEPENDS}"

FEATURE_PACKAGES_hipc-validation = "iperf linuxptp"
FEATURE_PACKAGES_hipc-validation:virtualization = "linuxptp"

require ${@bb.utils.contains('MACHINE', 'fvp-rd-kronos', 'conf/machine/include/fvp-rd-kronos-extras.inc', '', d)}

ZEPHYR_APP_SAFETY_ISLAND_CL0 = "bridge"
ZEPHYR_APP_SAFETY_ISLAND_CL0:hipc-validation = "zperf"
ZEPHYR_APP_SAFETY_ISLAND_CL0:si-psa-crypto-tests = "psa-crypto-tests"
ZEPHYR_APP_SAFETY_ISLAND_CL1 = "fault-mgmt"
ZEPHYR_APP_SAFETY_ISLAND_CL1:cam = "cam"
ZEPHYR_APP_SAFETY_ISLAND_CL1:hipc-validation = "zperf"
ZEPHYR_APP_SAFETY_ISLAND_CL1:si0-bridge-ethernet0 = "zperf"
ZEPHYR_APP_SAFETY_ISLAND_CL1:si-psa-crypto-tests = "psa-crypto-tests"
ZEPHYR_APP_SAFETY_ISLAND_CL2:actuation = "actuation"
ZEPHYR_APP_SAFETY_ISLAND_CL2:hipc-validation = "zperf"
ZEPHYR_APP_SAFETY_ISLAND_CL2:si0-bridge-ethernet0 = "zperf"
ZEPHYR_APP_SAFETY_ISLAND_CL2:si-psa-storage-tests = "psa-storage-tests"
ZEPHYR_APP_SAFETY_ISLAND_CL2:si-psa-crypto-tests = "psa-crypto-tests"

LINUXPTP_IFACES:cam = "ethsi1"
LINUXPTP_IFACES:append:cam:virtualization = " domu1.ethsi1 domu2.ethsi1"
LINUXPTP_IFACES:cam:domu = "ethsi1"
LINUXPTP_IFACES:hipc-validation = "ethsi0 ethsi1 ethsi2"
LINUXPTP_IFACES:append:hipc-validation:virtualization = \
    " domu1.ethsi0 domu2.ethsi0"
LINUXPTP_IFACES:hipc-validation:domu = "ethsi0"

TEST_SUITES_EXTRA ?= " \
    test_10_safety_island_c0 \
    test_10_safety_island_c1 \
    test_10_safety_island_c2 \
    "

TEST_SUITES_EXTRA:hipc-validation = " test_30_hipc test_30_ptp"

TEST_SUITES_EXTRA:actuation = " \
    test_30_actuation \
    test_10_fault_mgmt \
    test_10_safety_island_c2 \
    "

TEST_SUITES_EXTRA:hipc-validation:virtualization = " \
    test_30_hipc_virtualization \
    test_30_ptp \
    "

TEST_SUITES_EXTRA:si0-bridge-ethernet0 = " test_30_si0_bridge_ethernet0"

TEST_SUITES_EXTRA:cam = " test_40_cam"

TEST_SUITES_EXTRA:si-psa-storage-tests = " test_10_si_psa_arch_tests"
TEST_SUITES_EXTRA:si-psa-crypto-tests = " test_10_si_psa_arch_tests"

TEST_SUITES_EXTRA:append:actuation:virtualization = " \
    test_40_virtualization \
    "

TEST_SUITES:append = " \
    test_10_linuxlogin \
    test_40_parsec \
    test_40_sve \
    ${TEST_SUITES_EXTRA} \
"

TEST_SUITES:remove:si0-bridge-ethernet0 = "\
    test_00_lcp \
    test_00_trusted_firmware_a \
    test_10_linuxboot \
    fvp_devices \
    ssh \
    ping \
    test_10_linuxlogin \
    test_40_parsec \
    test_40_sve \
    "

TEST_SUITES:remove:hipc-validation = " \
    fvp_devices \
    ssh \
    ping \
    test_40_parsec \
    test_40_sve \
    "

TEST_SUITES:remove:virtualization = " \
    test_00_secure_partition \
    test_10_fault_mgmt \
    "

TEST_SUITES:remove:cam = "\
    fvp_devices \
    ssh \
    ping \
    test_40_parsec \
    test_40_sve \
    "

TEST_SUITES:remove:si-psa-storage-tests = "\
    fvp_devices \
    ssh \
    ping \
    test_40_parsec \
    test_40_sve \
    "

TEST_SUITES:remove:si-psa-crypto-tests = "\
    fvp_devices \
    ssh \
    ping \
    test_40_parsec \
    test_40_sve \
    "

EXTRA_TESTIMAGE_RDEPENDS ?= ""
EXTRA_TESTIMAGE_RDEPENDS:si0-bridge-ethernet0 = "iperf-native:do_populate_sysroot"

do_testimage[rdepends] += "${EXTRA_TESTIMAGE_RDEPENDS}"

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

OVERRIDES:append = "${@bb.utils.contains('EXTRA_IMAGE_FEATURES', \
                    'nosve', ':nosve', '', d)}"

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
    nosve \
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
    arm-auto-solutions-network-conf \
    openvswitch \
    packagegroup-core-boot \
    packagegroup-core-ssh-openssh \
    packagegroup-machine-base \
    packagegroup-security-parsec \
    rpmsg-net-mod \
    systemd-conf-arm-auto-solutions \
    systemd-ovs-arm-auto-solutions \
    ${CORE_IMAGE_EXTRA_INSTALL} \
    "

FEATURE_PACKAGES_baremetal = " \
    ${FEATURE_PACKAGES_COMMON} \
    packagegroup-ts-tests-psa \
    parsec-mbedtls-demo \
    "

FEATURE_PACKAGES_virtualization = " \
    ${FEATURE_PACKAGES_COMMON} \
    domu-package \
    xen-tools \
    virtualization-integration-tests-ptest \
    "

FEATURE_PACKAGES_domu = " \
    arm-auto-solutions-network-conf \
    packagegroup-core-boot \
    systemd-conf-arm-auto-solutions \
    ${CORE_IMAGE_EXTRA_INSTALL} \
    "

ACTUATION_PACKAGES ?= "actuation-player"
FEATURE_PACKAGES_actuation = "${ACTUATION_PACKAGES}"
FEATURE_PACKAGES_actuation:virtualization = ""

CAM_PACKAGES ?= "cam-app-example cam-tool linuxptp"
FEATURE_PACKAGES_cam = "${CAM_PACKAGES}"
FEATURE_PACKAGES_cam:virtualization = "linuxptp"

ARM_AUTO_SOLUTIONS_EXTRA_IMAGEDEPENDS = ""
ARM_AUTO_SOLUTIONS_EXTRA_IMAGEDEPENDS:actuation = "packet-analyzer-native:do_addto_recipe_sysroot"

EXTRA_IMAGEDEPENDS:append:baremetal = " ${ARM_AUTO_SOLUTIONS_EXTRA_IMAGEDEPENDS}"
EXTRA_IMAGEDEPENDS:append:virtualization = " ${ARM_AUTO_SOLUTIONS_EXTRA_IMAGEDEPENDS}"

FEATURE_PACKAGES_hipc-validation = "iperf linuxptp"
FEATURE_PACKAGES_hipc-validation:virtualization = "linuxptp"

require ${@bb.utils.contains('EXTRA_IMAGE_FEATURES', 'virtualization', "conf/distro/include/arm-auto-solutions-virtualization.inc", '', d)}

# Override the EWAOL defaults
VIRTUAL-RUNTIME_cloud_service = "no-cloud"
VIRTUAL-RUNTIME_security_provider:virtualization = "sw-provider"
VIRTUAL-RUNTIME_security_provider:domu = "sw-provider"

ZEPHYR_APP_SAFETY_ISLAND_CL0:actuation = "bridge"
ZEPHYR_APP_SAFETY_ISLAND_CL0:si0-bridge-ethernet0 = "bridge"
ZEPHYR_APP_SAFETY_ISLAND_CL0:hipc-validation = "zperf"
ZEPHYR_APP_SAFETY_ISLAND_CL0:si-psa-crypto-tests = "psa-crypto-tests"
ZEPHYR_APP_SAFETY_ISLAND_CL1:actuation = "fault-mgmt"
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

TEST_SUITES:cam = " \
    test_00_fwu \
    test_00_rse \
    test_00_secure_partition \
    fvp_boot \
    test_10_linuxboot \
    test_10_linuxlogin \
    test_40_cam \
    test_99_linuxshutdown \
    "
TEST_SUITES:remove:cam:virtualization = " \
    test_00_fwu \
    test_00_secure_partition \
    "

TEST_SUITES:actuation = " \
    ping \
    ssh \
    test_00_rse \
    test_00_secure_partition \
    fvp_boot \
    test_10_fault_mgmt \
    test_10_linuxboot \
    test_10_linuxlogin \
    test_10_safety_island_c2 \
    test_20_fvp_devices \
    test_40_parsec \
    test_40_sve \
    test_30_actuation \
    test_99_linuxshutdown \
    "
TEST_SUITES:append:actuation:virtualization = " \
    test_40_virtualization \
    "
TEST_SUITES:remove:actuation:virtualization = " \
    test_00_secure_partition \
    test_10_fault_mgmt \
    "

TEST_SUITES:hipc-validation = " \
    test_00_rse \
    test_00_secure_partition \
    fvp_boot \
    test_10_linuxboot \
    test_10_linuxlogin \
    test_30_hipc \
    test_30_ptp \
    test_99_linuxshutdown \
    "
TEST_SUITES_EXTRA:hipc-validation:virtualization = " \
    test_30_hipc_virtualization \
    test_30_ptp_virtualization \
    "

TEST_SUITES:si0-bridge-ethernet0 = " \
    test_00_rse \
    test_00_secure_partition \
    fvp_boot \
    test_30_si0_bridge_ethernet0 \
    "

TEST_SUITES:si-psa-storage-tests = " \
    test_00_rse \
    test_00_secure_partition \
    fvp_boot \
    test_10_linuxboot \
    test_10_linuxlogin \
    test_10_si_psa_arch_tests \
    test_99_linuxshutdown \
    "

TEST_SUITES:si-psa-crypto-tests = " \
    test_00_rse \
    test_00_secure_partition \
    fvp_boot \
    test_10_linuxboot \
    test_10_linuxlogin \
    test_10_si_psa_arch_tests \
    test_99_linuxshutdown \
    "

TEST_SUITES:nosve = " \
    fvp_boot \
    test_00_rse \
    test_00_secure_partition \
    test_10_linuxboot \
    test_10_linuxlogin \
    test_99_linuxshutdown \
    "

TEST_SUITES:cassini-test = " \
    ping \
    ssh \
    test_10_linuxboot \
    test_10_linuxlogin \
    test_40_ewaol \
    test_50_trusted_services \
    test_99_linuxshutdown \
    "

EXTRA_TESTIMAGE_RDEPENDS ?= ""
EXTRA_TESTIMAGE_RDEPENDS:si0-bridge-ethernet0 = "iperf-native:do_populate_sysroot"

do_testimage[rdepends] += "${EXTRA_TESTIMAGE_RDEPENDS}"

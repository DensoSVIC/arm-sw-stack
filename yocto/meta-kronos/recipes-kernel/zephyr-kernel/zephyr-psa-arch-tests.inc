#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend := "${THISDIR}/files/psa-arch-tests:"
ZEPHYR_PSA_ARCH_TESTS_PATCHDIR = "modules/tee/tf-m/psa-arch-tests"
ZEPHYR_PSA_ARCH_TESTS_PATCHES ?= "\
    file://0001-kronos-Add-zephyr-module-file.patch;patchdir=${ZEPHYR_PSA_ARCH_TESTS_PATCHDIR} \
    file://0002-kronos-Remove-test_s003-from-Secure-Storage-tests.patch;patchdir=${ZEPHYR_PSA_ARCH_TESTS_PATCHDIR} \
    file://0003-kronos-Re-add-test_s003-to-Secure-Storage-tests.patch;patchdir=${ZEPHYR_PSA_ARCH_TESTS_PATCHDIR} \
    file://0004-tests-zephyr-mbedtls-disable-failing-tests.patch;patchdir=${ZEPHYR_PSA_ARCH_TESTS_PATCHDIR} \
"
SRC_URI_ZEPHYR_PSA_ARCH_TESTS = "${ZEPHYR_PSA_ARCH_TESTS_PATCHES}"
SRC_URI_ZEPHYR_PSA_ARCH_TESTS += "git://github.com/ARM-software/psa-arch-tests.git;protocol=https"

# Use a newer tag of PSA arch tests (v23.06_API1.5_ADAC_EAC)
# to fix an incompatibility with newer versions of CMake
SRCREV_psa-arch-tests = "334e110528f975186e601fb35b6250e770635467"

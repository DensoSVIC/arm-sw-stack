#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend := "${THISDIR}/files/psa-arch-tests:"
ZEPHYR_PSA_ARCH_TESTS_PATCHDIR = "modules/tee/tf-m/psa-arch-tests"

# /* cSpell:disable */
ZEPHYR_PSA_ARCH_TESTS_PATCHES ?= "\
    file://0001-kronos-Add-zephyr-module-file.patch;patchdir=${ZEPHYR_PSA_ARCH_TESTS_PATCHDIR} \
    file://0002-kronos-Remove-test_s003-from-Secure-Storage-tests.patch;patchdir=${ZEPHYR_PSA_ARCH_TESTS_PATCHDIR} \
    file://0003-kronos-Re-add-test_s003-to-Secure-Storage-tests.patch;patchdir=${ZEPHYR_PSA_ARCH_TESTS_PATCHDIR} \
    file://0004-tests-zephyr-mbedtls-disable-failing-tests.patch;patchdir=${ZEPHYR_PSA_ARCH_TESTS_PATCHDIR} \
    file://0005-tests-zephyr-mbedtls-Make-asymmetric-operations-to-c.patch;patchdir=${ZEPHYR_PSA_ARCH_TESTS_PATCHDIR} \
    file://0006-Move-generate-key-to-end.patch;patchdir=${ZEPHYR_PSA_ARCH_TESTS_PATCHDIR} \
"
# /* cSpell:enable */

SRC_URI_ZEPHYR_PSA_ARCH_TESTS = "git://github.com/ARM-software/psa-arch-tests.git;protocol=https"

SRC_URI += "\
    ${SRC_URI_ZEPHYR_PSA_ARCH_TESTS};name=psa-arch-tests;nobranch=1;destsuffix=git/modules/tee/tf-m/psa-arch-tests \
    ${ZEPHYR_PSA_ARCH_TESTS_PATCHES}"

# Use a newer tag of PSA arch tests (v23.06_API1.5_ADAC_EAC)
# to fix an incompatibility with newer versions of CMake
SRCREV_psa-arch-tests = "334e110528f975186e601fb35b6250e770635467"

ZEPHYR_MODULES:prepend = "${S}/modules/tee/tf-m/psa-arch-tests\;"

#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "System tests"
DESCRIPTION = "A set of generic system tests using BATS"

HOMEPAGE = "https://kronos.docs.arm.com/"
LICENSE = "MIT"
# License file is in "layers/poky/meta/files/common-licenses".
# nooelint: oelint.var.licenseremotefile
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://tests"
S = "${WORKDIR}/tests"

inherit ptest

RDEPENDS:${PN} += "bats"

TESTS_PATH = "${libdir}/${BPN}"
COMPATIBLE_MACHINE = "fvp-rd-kronos"

# A set of tags to restrict the tests that run based on the current
# stack/machine
SYSTEM_TESTS_FILTER_TAGS_STACK = "${@bb.utils.contains(\
'EXTRA_IMAGE_FEATURES', 'virtualization', 'virtualization dom0', '', d)}"
# An optional set of extra tags, which can be specified globally to further
# restrict which tests run
SYSTEM_TESTS_FILTER_TAGS_EXTRA ?= ""
# The full list of test tags to pass to BATS
SYSTEM_TESTS_FILTER_TAGS ?= "${SYSTEM_TESTS_FILTER_TAGS_STACK} \
${SYSTEM_TESTS_FILTER_TAGS_EXTRA}"

BATS_EXTRA_OPTIONS ?= "\
    --verbose-run \
    --trace \
    --timing \
    --print-output-on-failure \
    --show-output-of-passing-tests"

do_configure[noexec] = "1"

do_compile() {
    cat <<EOF > ${WORKDIR}/run-ptest
#!/bin/sh
bats "${TESTS_PATH}/tests" --filter-tags \
"${@','.join(d.getVar('SYSTEM_TESTS_FILTER_TAGS').split())}" \
${BATS_EXTRA_OPTIONS}
EOF
}

do_install() {
    install -d ${D}${TESTS_PATH}
    cp -rf ${S} ${D}${TESTS_PATH}
}

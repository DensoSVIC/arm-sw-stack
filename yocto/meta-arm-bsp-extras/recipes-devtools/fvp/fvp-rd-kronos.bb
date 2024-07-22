#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

require recipes-devtools/fvp/fvp-ecosystem.inc

LIC_FILES_CHKSUM = "file://license_terms/license_agreement.txt;md5=1a33828e132ba71861c11688dbb0bd16 \
                    file://license_terms/third_party_licenses/third_party_licenses.txt;md5=58b552b918d097a8ba802168312d76b2 \
                    file://license_terms/third_party_licenses/arm_license_management_utilities/third_party_licenses.txt;md5=abcaafefc7b7a0cdf6664c51f9075c5b"

FVP_BUILD_NUMBER ?= "0.0.8357"
PV = "${FVP_BUILD_NUMBER}"

MODEL = "RD-Kronos"
MODEL_CODE = "FVP_RD_Kronos"

FVP_SERVER_URL ?= "https://developer.arm.com/-/media/Arm%20Developer%20Community/Downloads/OSS/FVP/Automotive%20FVPs"
FVP_SERVER_USER ?= ""
FVP_SERVER_KEY ?= ""

# Download URL contains ${PV_URL}, not ${PV}
# nooelint: oelint.vars.downloadfilename
SRC_URI = "${FVP_SERVER_URL}/${MODEL_CODE}_${PV_URL}_${FVP_ARCH}.tgz;user=${FVP_SERVER_USER};pswd=${FVP_SERVER_KEY};subdir=${BP};name=${BUILD_ARCH}"
SRC_URI[aarch64.sha256sum] = "7a4c034b8f905a09498c4907b5210bee7d95c17a5e6fd2b4220bf22a51df0f96"
SRC_URI[x86_64.sha256sum] = "9f7e75c678ba7639c13039466a984cb26554b9e03f4e8a95e526e24245808ae1"

# Mark no COMPATIBLE_HOST for the target, so that FVP related recipes can't be
# run inside the FVP.
COMPATIBLE_HOST = "(aarch64|x86_64).*-linux"
COMPATIBLE_HOST:class-target = "^$"

python() {
    if  d.getVar('FVP_OVERRIDE') == '1':
        d.setVarFlag('SRC_URI', 'aarch64.sha256sum', '')
        d.setVarFlag('SRC_URI', 'x86_64.sha256sum', '')
        d.setVar('BB_STRICT_CHECKSUM', 'ignore')
        d.setVar('LICENSE', 'CLOSED')
}

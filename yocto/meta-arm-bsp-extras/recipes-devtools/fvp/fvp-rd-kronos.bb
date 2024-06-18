#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

require recipes-devtools/fvp/fvp-ecosystem.inc

LIC_FILES_CHKSUM = "file://license_terms/license_agreement.txt;md5=1a33828e132ba71861c11688dbb0bd16 \
                    file://license_terms/third_party_licenses/third_party_licenses.txt;md5=58b552b918d097a8ba802168312d76b2 \
                    file://license_terms/third_party_licenses/arm_license_management_utilities/third_party_licenses.txt;md5=abcaafefc7b7a0cdf6664c51f9075c5b"

FVP_BUILD_NUMBER ?= "0.0.8294"
PV = "${FVP_BUILD_NUMBER}"

MODEL = "RD-Kronos"
MODEL_CODE = "FVP_RD_Kronos"

FVP_SERVER_URL ?= "https://developer.arm.com/-/media/Arm%20Developer%20Community/Downloads/OSS/FVP/Automotive%20FVPs"
FVP_SERVER_USER ?= ""
FVP_SERVER_KEY ?= ""

# Download URL contains ${PV_URL}, not ${PV}
# nooelint: oelint.vars.downloadfilename
SRC_URI = "${FVP_SERVER_URL}/${MODEL_CODE}_${PV_URL}_${FVP_ARCH}.tgz;user=${FVP_SERVER_USER};pswd=${FVP_SERVER_KEY};subdir=${BP};name=${BUILD_ARCH}"
SRC_URI[aarch64.sha256sum] = "ee3a349d27679864a3299a5d7ecd0b8c2dcc5f63f6007d160af305e1b3509e64"
SRC_URI[x86_64.sha256sum] = "feb2a4609f38ccb993a25ec2f8d9d322cb20854d358ff17c88b0113060023bac"

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

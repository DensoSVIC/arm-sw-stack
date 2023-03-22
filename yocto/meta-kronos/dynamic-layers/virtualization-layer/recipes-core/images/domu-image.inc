#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

IMAGE_INSTALL = ""
IMAGE_LINGUAS = ""

inherit core-image

IMAGE_OVERHEAD_FACTOR = "1.5"
IMAGE_FSTYPES = "wic.qcow2"

WKS_FILE = "domu-disk.wks"

inherit features_check
REQUIRED_IMAGE_FEATURES = "domu"
CONFLICT_IMAGE_FEATURES = "baremetal virtualization"
COMPATIBLE_MACHINE = "generic-arm64"

DOMU_HOSTNAME ?= "domu"

update_hostname() {
    echo "${DOMU_HOSTNAME}" >  ${IMAGE_ROOTFS}/etc/hostname
    sed -i "s/127.0.1.1.*${MACHINE}/127.0.1.1 ${DOMU_HOSTNAME}/g"\
           ${IMAGE_ROOTFS}/${sysconfdir}/hosts
}

ROOTFS_POSTPROCESS_COMMAND += "update_hostname; "

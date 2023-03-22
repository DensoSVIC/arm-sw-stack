#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "Xen domu2 image"
DESCRIPTION = "An image recipe, based on core-image, which builds a qcow2 disk\
image for a DomU without a boot partition"

python() {
    domu_instances = d.getVar("DOMU_INSTANCES")
    if (not domu_instances or
        0 < int(domu_instances) < 2):
        raise bb.parse.SkipRecipe(
              f"DOMU_INSTANCES=\"{domu_instances}\" not correctly set")
}

DOMU_HOSTNAME = "domu2"

require domu-image.inc

#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

ZEPHYR_TFM_PATCHDIR = "modules/tee/tf-m/trusted-firmware-m"
ZEPHYR_TFM_PATCHES ?= "\
    file://trusted-firmware-m/0001-interface-Zephyr-psa-arch-tests-TF-M-PSA-interfaces-.patch;patchdir=${ZEPHYR_TFM_PATCHDIR} \
"
SRC_URI += "${ZEPHYR_TFM_PATCHES}"

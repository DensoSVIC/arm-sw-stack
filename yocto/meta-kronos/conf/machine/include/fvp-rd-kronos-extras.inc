#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

PREFERRED_VERSION_zephyr-kernel = "3.3.0"
ZEPHYR_TOOLCHAIN_VARIANT = "zephyr"

SAFETY_ISLAND_C0_IMAGE ??= "zephyr-app-si-cl0"
SAFETY_ISLAND_C1_IMAGE ??= "zephyr-app-si-cl1"
SAFETY_ISLAND_C2_IMAGE ??= "zephyr-app-si-cl2"

TEST_SUITES += "test_10_safety_island_c0"

require ${@bb.utils.contains('EXTRA_IMAGE_FEATURES', 'virtualization', "${KRONOS_VIRTUALIZATION_DYNAMIC_DIR}/conf/machine/include/fvp-rd-kronos-xen.inc", '', d)}

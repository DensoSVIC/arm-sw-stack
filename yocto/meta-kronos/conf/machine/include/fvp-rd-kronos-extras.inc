#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

ZEPHYR_INHERIT_CLASSES:append = " zephyr-machine-extras"
ZEPHYR_TOOLCHAIN_VARIANT = "zephyr"

SAFETY_ISLAND_C0_RECIPE ??= "zephyr-app-si-cl0"
SAFETY_ISLAND_C1_RECIPE ??= "zephyr-app-si-cl1"
SAFETY_ISLAND_C2_RECIPE ??= "zephyr-app-si-cl2"

SAFETY_ISLAND_C0_IMAGE ??= "${SAFETY_ISLAND_C0_RECIPE}"
SAFETY_ISLAND_C1_IMAGE ??= "${SAFETY_ISLAND_C1_RECIPE}"
SAFETY_ISLAND_C2_IMAGE ??= "${SAFETY_ISLAND_C2_RECIPE}"

FVP_SI0_ETHERNET0_HOST_NETPORT ?= "6001"
FVP_CONFIG[css.sysctrl.si.ethernet0.enabled] ?= "1"
FVP_CONFIG[css.sysctrl.si.hostbridge0.userNetworking] ?= "1"
FVP_CONFIG[css.sysctrl.si.hostbridge0.userNetPorts] ?= \
        "${FVP_SI0_ETHERNET0_HOST_NETPORT}=172.20.51.1:5001"

TEST_FVP_LINUX_BOOT_TIMEOUT = "${@30*60}"

require ${@bb.utils.contains('EXTRA_IMAGE_FEATURES', 'virtualization', "conf/machine/include/fvp-rd-kronos-virtualization.inc", '', d)}

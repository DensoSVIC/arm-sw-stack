#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

PREFERRED_PROVIDER_virtual/kernel = "linux-yocto-rt"

PREFERRED_VERSION_zephyr-kernel = "3.5.0"
ZEPHYR_INHERIT_CLASSES:append = " zephyr-arm-safety-island"
ZEPHYR_TOOLCHAIN_VARIANT = "zephyr"

SAFETY_ISLAND_C0_RECIPE ??= "zephyr-app-si-cl0"
SAFETY_ISLAND_C1_RECIPE ??= "zephyr-app-si-cl1"
SAFETY_ISLAND_C2_RECIPE ??= "zephyr-app-si-cl2"

SAFETY_ISLAND_C0_IMAGE ??= "${SAFETY_ISLAND_C0_RECIPE}"
SAFETY_ISLAND_C1_IMAGE ??= "${SAFETY_ISLAND_C1_RECIPE}"
SAFETY_ISLAND_C2_IMAGE ??= "${SAFETY_ISLAND_C2_RECIPE}"

FVP_SI1_BRIDGED_HOST_NETPORT ?= "6002"
FVP_SI2_BRIDGED_HOST_NETPORT ?= "6003"
FVP_ACTUATION_HOST_ANALYZER_PORT ?= "49152"
FVP_CONFIG[css.sysctrl.si.ethernet0.enabled] ?= "1"
FVP_CONFIG[css.sysctrl.si.hostbridge0.userNetworking] ?= "1"
FVP_CONFIG[css.sysctrl.si.hostbridge0.userNetSubnet] ?= "192.168.10.0/24"
FVP_CONFIG[css.sysctrl.si.hostbridge0.userNetPorts] ?= "\
        ${FVP_SI1_BRIDGED_HOST_NETPORT}=192.168.10.1:5001, \
        ${FVP_SI2_BRIDGED_HOST_NETPORT}=192.168.10.2:5001, \
        ${FVP_ACTUATION_HOST_ANALYZER_PORT}=192.168.10.2:49152 \
"

AHCI_IMAGE_PATH ?= ""
FVP_CONFIG[pcie_group_0.pcie4.pcie_rc.ahci0.ahci.image_path] = "${AHCI_IMAGE_PATH}"

TEST_FVP_LINUX_BOOT_TIMEOUT = "${@60*60}"

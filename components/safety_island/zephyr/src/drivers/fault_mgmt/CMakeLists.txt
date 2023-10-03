#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: Apache-2.0

zephyr_library()

if(CONFIG_FAULT_MGMT_ARM_FMU)

zephyr_library_sources(
        fault_mgmt_arm_fmu_common.c
        fault_mgmt_arm_fmu_gic.c
        fault_mgmt_arm_fmu_system.c
)
zephyr_linker_sources(DATA_SECTIONS fault_mgmt_arm_fmu.ld)

endif()

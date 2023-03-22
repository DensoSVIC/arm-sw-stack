..
 # SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 # affiliates <open-source-office@arm.com></text>
 #
 # SPDX-License-Identifier: MIT

############
Boot Process
############

.. _design_boot_process_primary_compute_boot_flow:

*************************
Primary Compute Boot Flow
*************************

The Primary Compute is the Application Processor in the Kronos Reference
Design. The purpose of its firmware is to provide an |Arm SystemReadyTM| IR
aligned interface to Linux. |Arm SystemReadyTM| IR compatible systems are
required to follow the |Device Tree specification|_, so the
:ref:`design_components_u-boot` bootloader is used in the non-secure world,
which provides the UEFI implementation and exposes the device tree to Linux.

:ref:`design_components_trusted-firmware-a` provides the initial, secure-world
firmware, which consists of BL2 and BL31. BL33 is provided by U-Boot.

The Primary Compute boot flow follows the following steps:

1. AP BL2:

   * Copies AP BL31 and BL33 from flash to SRAM and DRAM
   * Jumps to AP BL31

2. AP BL31 starts AP BL33
3. AP BL33 starts the Linux operating system

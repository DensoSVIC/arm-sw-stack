..
 # SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 # affiliates <open-source-office@arm.com></text>
 #
 # SPDX-License-Identifier: MIT

############
Boot Process
############

.. _design_boot_process_rss-oriented_boot_flow:

**********************
RSS-oriented Boot Flow
**********************

The :ref:`design_components_rss` is the root of the trust chain. It is the
first booting element when the system is powered up.

The RSS, implemented in Trusted Firmware-M (TF-M), has 3 boot stages. The
images for each stage are stored in different media:

* RSS BL1 (corresponds to TF-M BL11) is in ROM
* RSS BL2 (TF-M BL12) is in OTP
* RSS BL3 (TF-M BL2) is in NVM flash

The NVM flash contains not only RSS BL3, but also other images that are booted
by the RSS. The images currently included in the flash are:

* RSS BL3
* RSS Runtime
* SCP RAM Firmware (SCP RAMFW)
* LCP RAM Firmware (LCP RAMFW)
* Safety Island Cluster 0 (SI CL0)
* Safety Island Cluster 1 (SI CL1)
* Safety Island Cluster 2 (SI CL2)
* Application Processor BL2 (AP BL2)

:ref:`design_components_scp-firmware` has been extended to additionally
synchronize power control with the loading of images by the RSS.

The following diagram illustrates the boot flow that originates from the RSS.

|

.. image:: ../images/rss_oriented_boot_flow.svg
   :align: center

|

Major steps of the boot flow:

1. RSS BL1 begins executing in place from ROM when the system is powered up. It:

   * Copies RSS BL2 from OTP to SRAM
   * Verifies RSS BL2 against the hash stored in OTP
   * Jumps to RSS BL2, if the hash verification has succeeded

2. RSS BL2:

   * Copies RSS BL3 image from flash into SRAM
   * Verifies RSS BL3 image using asymmetric cryptography
   * Jumps to RSS BL3, if the image was successfully verified

3. RSS BL3:

   * Copies SCP RAMFW from flash to SCP SRAM and verifies the image
   * Resets the SCP
   * Copies SI CL0 from flash to SI LLRAM and verifies the image
   * Notifies the SCP to power on the SI CL0
   * Copies SI CL1 from flash to SI LLRAM and verifies the image
   * Notifies the SCP to power on the SI CL1
   * Copies SI CL2 from flash to SI LLRAM and verifies the image
   * Notifies the SCP to power on the SI CL2
   * Copies LCP from flash to LCP SRAM and verifies the image
   * Release the LCP from reset
   * Copies AP BL2 from flash to AP SRAM and verifies the image
   * Notifies the SCP to power on the AP

.. _design_boot_process_primary_compute_boot_flow:

*************************
Primary Compute Boot Flow
*************************

The Primary Compute is the Application Processor in the Kronos Reference
Design. The purpose of its firmware is to provide an |Arm SystemReadyTM| IR
aligned interface to Linux. |Arm SystemReadyTM| IR compatible systems are
required to follow the `Device Tree specification`_, so the
:ref:`design_components_u-boot` bootloader is used in the non-secure world,
which provides the UEFI implementation and exposes the device tree to Linux.

:ref:`design_components_trusted-firmware-a` provides the initial, secure-world
firmware, which consists of BL2 and BL31. BL33 is provided by U-Boot.

The Primary Compute boot flow follows the following steps:

1. AP BL2:

   * Copies AP BL31 and BL33 from flash to SRAM and DRAM
   * Jumps to AP BL31

2. AP BL31 starts AP BL33 (U-Boot)
3. AP BL33 loads GRUB2 from the boot partition
4. Grub loads and boots either Linux (Baremetal Architecture) or Xen
   (Virtualization Architecture) from the boot partition, depending on the Grub
   configuration

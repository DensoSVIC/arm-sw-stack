..
 # SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 # affiliates <open-source-office@arm.com></text>
 #
 # SPDX-License-Identifier: MIT

.. _design_components:

##########
Components
##########

The stack comprises of the following components:

.. list-table::
  :header-rows: 1

  * - Component
    - Version
    - Source
  * - Trusted Firmware-M (:ref:`design_components_rss`)
    - |Trusted Firmware-M version| (based on |Trusted Firmware-M base version|)
    - `Trusted Firmware-M repository`_
  * - :ref:`design_components_scp-firmware`
    - |SCP-Firmware version| (based on |SCP-Firmware base version|)
    - `SCP-Firmware repository`_
  * - :ref:`design_components_trusted-firmware-a`
    - |Trusted Firmware-A version|
    - `Trusted Firmware-A repository`_
  * - :ref:`design_components_u-boot`
    - |U-Boot version|
    - `U-Boot repository`_
  * - :ref:`design_components_linux`
    - |Linux version|
    - `Linux repository`_

.. _design_components_rss:

***
RSS
***

.. _design_components_rss_downstream_changes:

Downstream Changes
==================

.. _design_components_scp-firmware:

************
SCP-firmware
************

.. _design_components_scp-firmware_downstream_changes:

Downstream Changes
==================

***************
Primary Compute
***************

.. _design_components_trusted-firmware-a:

Trusted Firmware-A
==================

Trusted Firmware-A is the initial bootloader on the Primary Compute.

For RD-Kronos, the initial TF-A boot stage is BL2, which runs from a known
address at EL3, using the ``BL2_AT_EL3`` compilation option. This option has
been extended for RD-Kronos to load the FW_CONFIG for dynamic configuration (a
role typically performed by BL1). BL2 is responsible for loading the subsequent
boot stages and their configuration files from the FIP flash image. This flash
image contains:

 * BL31
 * BL33 (:ref:`design_components_u-boot`)
 * The HW_CONFIG device tree
 * The TB_FW_CONFIG device tree

The device tree for the Primary Compute of the RD-Kronos FVP is compiled by
Trusted Firmware-A, bundled in the Primary Compute flash image (as the
HW_CONFIG) at rest and used to configure :ref:`design_components_u-boot`, Linux
and Xen at runtime. It is located at
:meta-arm-repo:`meta-arm-bsp/recipes-bsp/trusted-firmware-a/files/fvp-rd-kronos/rdkronos.dts`.

.. _design_components_trusted-firmware-a_downstream_changes:

Downstream Changes
------------------

Patch files can be found at
:meta-arm-repo:`meta-arm-bsp/recipes-bsp/trusted-firmware-a/files/fvp-rd-kronos/`
to:

 * Implement the RD-Kronos platform port, based on RD-Fremont.
 * Compile the HW_CONFIG device tree and add it to the FIP image.
 * Extend BL2_AT_EL3 to load the FW_CONFIG for dynamic configuration.

.. _design_components_u-boot:

U-Boot
======

U-Boot is the non-secure world second-stage bootloader (BL33 in TF-A) on the
Primary Compute. It consumes the HW_CONFIG device tree provided by
Trusted Firmware-A and provides UEFI services to UEFI applications like Linux
and Xen. The device tree is used to configure U-Boot at runtime, minimizing the
need for platform-specific configuration.

.. _design_components_u-boot_downstream_changes:

Downstream Changes
------------------

The implementation is based on the VExpress64 board family. Patch
files can be found at
:meta-arm-repo:`meta-arm-bsp/recipes-bsp/u-boot/u-boot/fvp-rd-kronos/`
to:

 * Consume the device tree using register x1, the TF-A default.
 * Provide a minimal, generic defconfig for FVPs, vexpress_fvp_defconfig.
 * Enable the real-time clock for the VExpress64 boards by default.
 * Use OF_HAS_PRIOR_STAGE for the BASE_FVP configuration, to indicate the
   origin of the device tree.

.. _design_components_linux:

Linux Kernel
============

.. _design_components_linux_downstream_changes:

Downstream Changes
------------------

**********
References
**********

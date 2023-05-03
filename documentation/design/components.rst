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
  * - :ref:`design_components_xen`
    - |Xen version|
    - `Xen repository`_
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

.. _design_components_xen:

Xen
===

Xen is a type-1 hypervisor, providing services that allow multiple computer
operating systems to execute on the same computer hardware concurrently.
Responsibilities of the Xen hypervisor include memory management and CPU
scheduling of all virtual machines (domains), and for launching the most
privileged domain (Dom0) - the only virtual machine which by default
has direct access to hardware. From the Dom0 the hypervisor can be managed
and unprivileged domains (DomU) can be launched.

On starting up, the GRUB2 configuration uses the "chainloader" command to
instruct the UEFI services provider (U-boot) to load and run Xen as an EFI
application. Further Xen reads its configuration (xen.cfg) from the boot
partition of the virtio disk containing the boot arguments for Xen and Dom0
to start the whole system.

The `Arm Memory Partitioning and Monitoring`_ (MPAM) extension is enabled in
Xen. MPAM is an optional extension to Armv8.4 and later versions. It
defines a method that software can utilize to apportion and monitor the
performance-giving resources (usually cache and memory bandwidth) of the
memory system. Domains can be assigned with dedicated system level cache (SLC)
slices so that cache contention with multiple domains can be mitigated.

|

.. image:: ../images/xen_mpam_structure.svg
   :align: center

|

The stack offers several methods for users to configure MPAM for domains:

 * For Dom0, an optional Xen command line parameter ``dom0_mpam`` can be used
   to configure the cache portion bit mask (CPBM) for Dom0. The format of the
   ``dom0_mpam`` parameter is:

   .. code-block:: console

     dom0_mpam=slc:<CPBM in hexadecimal>

   To use the ``dom0_mpam`` parameter, users can add this parameter to the
   ``options`` of the ``[xen]`` section in xen.cfg config file. An example to
   assign the first 4 portions of SLC to Dom0 at Xen boot time is shown below:

   .. code-block:: console

     [xen]
     options=(...) dom0_mpam=slc:0xf

 * There is a set of sub-commands in "xl" to allow users to use MPAM at runtime.
   Users can use the ``xl psr-hwinfo`` command to query the system information
   of MPAM, and use ``xl psr-cat-set`` or ``xl psr-cat-show`` to configure or
   read the CPBM for Dom0 and DomU at runtime.

   The format of ``xl psr-cat-set`` is (``-l 0`` refers to SLC):

   .. code-block:: console

     xl psr-cat-set -l 0 <Domain ID> <CPBM in hexadecimal>

   The format of ``xl psr-cat-show`` is (``-l 0`` refers to SLC):

   .. code-block:: console

     xl psr-cat-show -l 0

   More detailed information of the sub-commands, please refer to the ``--help``
   of each sub-command respectively.

Xen is only included in the Virtualization Reference Stack Architecture.
Limitations of MPAM support in Xen include:

 * Currently, MPAM support in Xen is available for the system level cache (SLC)
   partitioning only.
 * In the Virtualization Reference Stack Architecture, DomU MPAM settings can
   only be manipulated by xl after the DomU has been created and started.
 * The FVP only provides the programmer's view of MPAM. There is no functional
   behaviour change implemented.

.. _design_components_xen_downstream_changes:

Downstream Changes
------------------
Patches for the Xen MPAM extension support at
:kronos-repo:`yocto/meta-kronos/dynamic-layers/virtualization-layer/recipes-extended/xen/files/`
to:

 * Discover MPAM CPU feature
 * Initialize MPAM at Xen boot time
 * Support MPAM in Xen tools to apply the domain MPAM configuration in
   userspace at runtime

.. _design_components_linux:

Linux Kernel
============

Remoteproc
----------

In Linux, a remoteproc driver for the Safety Island is added to the Linux
kernel. It is used to support RPMsg communication between the Armv9.0-A
cores and the Safety Island. More details on the communication can be
found in the :ref:`HIPC <design/hipc:Heterogeneous Inter-processor Communication (HIPC)>` section.

Virtual Network over RPMsg
--------------------------

In order to allow applications to access the remote processor using network
sockets, a virtual network device over RPMsg is introduced. The ``rpmsg_net``
kernel module is added for creating a virtual network device and converting
RPMsg data to network data.

.. _design_components_linux_downstream_changes:

Downstream Changes
------------------

The arm_si_rproc and rpmsg_net drivers can be found at
:kronos-repo:`components/primary_compute/linux_drivers`.

*************
Safety Island
*************

.. _design_components_zephyr:

Zephyr
======

`Zephyr`_ is an open source real-time operating system based on a small
footprint kernel designed for use on resource-constrained and embedded systems.

The Reference Stack uses Zephyr |zephyr version| as a baseline and introduces a
new board ``fvp_rd_kronos_safety_island`` for the Kronos FVP. It reuses the
``fvp_aemv8r`` SoC support and adds a pair of patches for MPU device region
configuration.

The Zephyr image for this board is running on the Safety Island clusters.
In order to enable communication with Armv9-A cores, a set of drivers
are added into Zephyr by means of an out-of-tree module. More details on the
communication can be found in the :ref:`HIPC <design/hipc:Heterogeneous Inter-processor Communication (HIPC)>` section.

MHUv3
-----

The Arm Message Handling Unit Version 3 (MHUv3) is a mailbox controller for
inter-processor communication. In the Kronos FVP, there are MHUv3 devices
on-chip for signaling between Armv9-A and Safety Island clusters, using the
doorbell protocol. A driver is added into the Zephyr inter-processor mailbox
framework to support this device.

Virtual Network over RPMsg
--------------------------

A ``veth_rpmsg`` driver is added for network socket based communication between
Armv9-A and Safety Island clusters. It implements an RPMsg backend by the OpenAMP
library and an adaptation layer for converting RPMsg data to network data.

Zperf sample
------------

The `zperf sample`_ can be used to stress test inter-processor communication
over a virtual network on the Kronos FVP. The board overlay dts and
configuration file are added to this sample. This sample needs to be used
together with iperf on the Armv9-A side for network performance testing.

.. _design_components_zephyr_downstream_changes:

Downstream Changes
------------------

The board support for ``fvp_rd_kronos_safety_island`` is located at
:kronos-repo:`components/safety_island/zephyr/src/boards/arm64/fvp_rd_kronos_safety_island`.

The out-of-tree driver for virtual network over RPMsg is located at
:kronos-repo:`components/safety_island/zephyr/src/drivers/ethernet`.

The out-of-tree driver for MHUv3 device is located at
:kronos-repo:`components/safety_island/zephyr/src/drivers/ipm`.

The MPU region configuration patch is located at
:kronos-repo:`yocto/meta-kronos/recipes-kernel/zephyr-kernel/files/zephyr`.

The zperf and networking related patch is located at
:kronos-repo:`yocto/meta-kronos/recipes-kernel/zephyr-kernel/files/zephyr`.

**********
References
**********

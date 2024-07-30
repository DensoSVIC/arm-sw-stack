..
 # SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
 # affiliates <open-source-office@arm.com></text>
 #
 # SPDX-License-Identifier: MIT

############
Boot Process
############

.. _design_boot_process_rse-oriented_boot_flow:

**********************
RSE-oriented Boot Flow
**********************

The :ref:`design_components_rse` is the root of trust chain. It is the
first booting element when the system is powered up.

The boot sequence is shown in the RSE-oriented Boot Flow diagram below
:ref:`design_boot_process_boot_flow` section.

The RSE uses a NVM flash to store the images of various components, including:

* RSE BL2 image
* RSE Runtime image
* SCP RAM Firmware (SCP RAMFW) image
* Safety Island Cluster 0 (SI CL0) image
* Safety Island Cluster 1 (SI CL1) image
* Safety Island Cluster 2 (SI CL2) image
* Application Processor BL2 (AP BL2) image

Trust Chain
===========

To make the platform secure, it is critical to protect each booting component
from the execution of malicious code. This is implemented by building a trust
chain where each step in the execution chain authenticates the next step before
execution.

The authentication of the images is done with hash (SHA-256) and digital
signature (RSA-3072) validation.

Image Signing
=============

A RSA private key is stored in TF-M's source code repository (the
``bl2/ext/mcuboot/root-RSA-3072.pem`` file) for testing. The private key is used
to sign the images listed above that RSE BL2 loads.

In the Yocto build stage of the platform, a shell function
``sign_host_image()`` is used to sign the images, which can be found at
:meta-arm-repo:`meta-arm/classes/tfm_sign_image.bbclass`.
Then the signed images are written to the NVM flash.

System Provisioning and Image Authentication
============================================

A public key is derived from the private key for authenticating the signed
images. The public key is also known as the Root of Trust Public Key (ROTPK). It
is also written in the NVM flash in the build stage. The hash of the public key
is written in the ``dm_dummy_provisioning_data.c`` file of the TF-M source code
folder ``platform/ext/target/arm/rse/common/provisioning/bundle_dm/``.

During the system's first boot, the hash of the public key is provisioned into
the OTP by BL1_1. More details on the provisioning can be found in the
`RSE provisioning`_ page. Once the provisioning stage has been completed, the
OTP contents cannot be updated.

BL2 reads the public key from the NVM flash and validates the public key against
the hash that has been provisioned in the OTP. Then BL2 uses the public key to
authenticate the images.

Key Customization
=================

The default private key used platform should only be used for test purposes.
Since this private key is widely distributed, it should never be used for
production. To replace the default key, the user needs to:

* Generate a new RSA key pair
* Replace the default private key ``bl2/ext/mcuboot/root-RSA-3072.pem`` with
  the new private key
* Generate the hash of the public key and replace the definition of
  ``ASSEMBLY_AND_TEST_PROV_DATA_KIND_0`` in ``dm_dummy_provisioning_data.c``
  with the hash value.

For details of how to generate the private key and the hash of the public key,
refer to the documentation of `imgtool`_ which is provided by MCUboot.

.. _design_boot_process_boot_flow:

Boot Flow
=========

The following diagram illustrates the boot flow that originates from the RSE.

|

.. image:: ../images/rse_oriented_boot_flow.*
   :align: center
   :alt: RSE-oriented Boot Flow

|

Major steps of the boot flow:

1. RSE BL1_1:

   * Begins executing in place from ROM when the system is powered up
   * Provisions RSE BL1_2 and various keys and other data from the provisioning
     bundle to the OTP (This step only happens on the system's first boot)
   * Copies the RSE BL1_2 image from the OTP to the SRAM
   * Validates RSE BL1_2 against the hash stored in the OTP
   * Transfers the execution to RSE BL1_2

2. RSE BL1_2:

   * Copies the encrypted RSE BL2 image from flash into the SRAM
   * Decrypts the RSE BL2 image
   * Transfers the execution to RSE BL2

3. RSE BL2:

   * Copies the SCP RAMFW image from flash to SCP SRAM and authenticates the
     image
   * Releases the SCP out of reset
   * Copies the SI CL0 image from flash to SI LLRAM and authenticates the image
   * Notifies the SCP to power on the SI CL0
   * Copies the SI CL1 image from flash to SI LLRAM and authenticates the image
   * Notifies the SCP to power on the SI CL1
   * Copies the SI CL2 image from flash to SI LLRAM and authenticates the image
   * Notifies the SCP to power on the SI CL2
   * Copies the AP BL2 image from flash to AP SRAM and authenticates the image
   * Notifies the SCP to power on the AP

.. _design_boot_process_primary_compute_boot_flow:

*************************
Primary Compute Boot Flow
*************************

The Application Processor (AP) refers to the cores in the Primary Compute of
the Reference Design. The purpose of its firmware is to provide an Arm
SystemReady IR-aligned interface to Linux. Arm SystemReady IR compatible systems
are required to follow the `Device Tree specification`_, so the
:ref:`design_components_u-boot` bootloader is used in the Normal world, which
provides the UEFI implementation and exposes the device tree to Linux.

:ref:`design_components_trusted-firmware-a` provides the initial, Secure world
firmware, which consists of BL2 and BL31. BL32 is provided by OP-TEE.
BL33 is provided by U-Boot.

The Primary Compute uses:

* Secure Flash to store the following components:

  * AP BL31
  * AP BL32 (OP-TEE)
  * AP BL33 (U-Boot)

* First VFAT (boot) partition of the VirtIO Block to store the following
  components:

  * GRUB2
  * Linux (Baremetal Architecture)
  * Xen (Virtualization Architecture)

The primary compute boot flow follows the following steps, and the boot
process complies with the `Trusted Board Boot Requirements (TBBR)`_:

1. AP BL2:

   * Copies the AP BL31 image from Secure Flash to Secure RAM
   * Copies the AP BL32 (OP-TEE) image from Secure Flash to Secure DRAM
   * Copies the AP BL33 (U-Boot) image from Secure Flash to Normal DRAM
   * Transfers the execution to AP BL31

2. AP BL31:

   * Transfers the execution to AP BL32 and then transfers the execution to
     AP BL33

3. AP BL33 loads GRUB2 from the boot partition

4. Grub loads and boots either Linux (Baremetal Architecture) or Xen
   (Virtualization Architecture) from the boot partition, depending on the Grub
   configuration

Trusted Board Boot (TBB)
========================

The Trusted Board Boot (TBB) feature ensures that only authenticated TF-A stages
and configs up to and including the BL33 boot-loader are allowed to run on the
platform. It achieves this by establishing a Chain of Trust to prevent the execution
of malicious firmware.

The TBB Chain of Trust (CoT) starts with a set of implicitly trusted components.
On the Arm development platforms, these components are:

   * An SHA-256 hash of the Root of Trust Public Key (ROTPK). It is stored in the
     trusted root-key storage registers. In the Arm Automotive Solutions Software
     Reference Stack design, the ROTPK and hash can be found in
     plat/arm/board/common/rotpk/ folder of TF-A and is intended for development
     purposes. This SHA256 hash of the ROTPK is embedded into the AP_BL2 image.
   * The RSE uses its chain of trust to authenticate and load the AP_BL2 image.

The remaining components in the CoT are certificates and boot loader images. The
certificates follow the `X.509 v3`_ standard. It supports adding custom extensions to
the certificates, which are used to store essential information to establish the CoT.

Please refer to the `Trusted Board Boot Requirements (TBBR)`_ and `Trusted Board Boot (TBB)`_
documentation to learn about the TBB implementation.

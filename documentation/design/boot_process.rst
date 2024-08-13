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

The boot sequence is shown in the RSE-oriented Boot Flow diagram in the Boot 
Flow section :ref:`design_boot_process_boot_flow` section.

The RSE uses an NVM flash to store the images of various components, including:

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

An RSA private key is stored in TF-M's source code repository (the
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

The default private key used in the Kronos platform should only be used for test
purposes. Since this private key is widely distributed, it should never be used
for production. To replace the default key, the user needs to:

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
     bundle to the OTP (this step only happens on the system's first boot)
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

Refer to the `Trusted Board Boot Requirements (TBBR)`_ and `Trusted Board Boot (TBB)`_
documentation to learn about the TBB implementation.

.. _design_boot_process_uefi_secure_boot:

UEFI Secure Boot
================

.. note::

  The UEFI Secure Boot feature is enabled for baremetal use cases only.

Introduction
~~~~~~~~~~~~

The Reference Software Stack implements UEFI Secure Boot as a method of
verification to ensure only trusted software is booted by the BL33 boot-loader
during the boot process. It establishes trust relationships between the platform
owner, platform firmware and operating systems. This prevents any malicious or
tampered software from being run.

In the Reference Software Stack, UEFI Secure Boot is implemented through the
provisioning and enrollment of authenticated variables. These authenticated
variables are:

  * Platform Key (PK): PK is the public and private key pair used to create and
    verify digital signatures. The private key is owned by the platform owner,
    typically stored off-device, and used to create digital signatures for UEFI
    executables, as well as enroll the Key Exchange Keys. It establishes the
    trust relationship between the platform and the platform owner. The public
    key is enrolled in the platform firmware and used to verify digital
    signatures.
  * Key Exchange Key (KEK): KEK signs and verifies writes to the Authorized and
    Forbidden Signature Databases. It establishes a trust relationship between
    the operating system and platform firmware, allowing the secure interchange
    of key information between the two.
  * Authorized Signature Database (db): db contains the list of signatures of
    trusted software which can be executed during the boot process.
  * Forbidden Signature Database (dbx): dbx contains the list of signatures that
    are no longer trusted to prevent software with these signatures from being
    executed. If the signature of a binary is present in both db and dbx, the
    binary should be forbidden.

Architecture
~~~~~~~~~~~~

In the Reference Software Stack, UEFI Secure Boot relies on the support of the
authenticated variables via the Trusted Services SMM Gateway SP and the RSE
Protected Storage service. See :ref:`design_secure_services` for more
information.

During the system build stage, the following 4 authenticated variable files are
deployed in the `uefi-sb-authenticated-variables` folder of the booting
partition:

* PK.auth
* KEK.auth
* db.auth
* dbx.auth

These authenticated variable files are used only for test purpose, they must not
be used for production.

|

.. image:: ../images/uefi_secure_boot_baremetal.*
   :align: center
   :alt: UEFI Secure Boot

|

The diagram illustrates the UEFI Secure Boot process in the Reference Software
Stack:

1. Enrollment and provisioning of the authenticated variables. This is only done
   on the first boot.

   * U-Boot loads the authenticated variables from the boot partition.
   * U-Boot calls SMM Gateway SP for the `SMM Variable Service`_.
   * SMM Gateway calls SE Proxy SP.
   * SE Proxy SP calls the RSE's Protected Storage service via MHUv3. The
     service is used to store the authenticated variables.

  2. U-Boot loads the signed GRUB image from the boot partition and verifies it.

  3. GRUB boots Linux using U-Boot services. The signed Linux kernel image is
     verified by U-Boot.

.. note::

  In some manual test steps in the :ref:`user_guide_reproduce_use_cases`
  section, the firmware flash images need to be recreated with the following
  command:

  `kas shell -c "bitbake firmware-fvp-rd-kronos -C deploy"`.

  This will clear all the data in the RSE Protected Storage including the
  enrolled authenticated variables. The authenticated variable files in the boot
  partition are not deleted after the first boot, so they can be enrolled again
  in the next boot.

  This is only for the convenience of demonstrating the Reference Software
  Stack. In a production environment, the authenticated variable files must be
  removed for the sake of security.

Please refer to Chapter `Secure Boot and Driver Signing` of the
`UEFI Specification`_ to learn more about UEFI Secure Boot.

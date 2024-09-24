..
 # SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
 # affiliates <open-source-office@arm.com></text>
 #
 # SPDX-License-Identifier: MIT

.. _design_secure_services:

###############
Secure Services
###############

************
Introduction
************

The Reference Software Stack provides the implementation of Secure Services
through both the Primary Compute and Safety Island. These services are aligned
to the following specifications:

* `PSA Crypto API`_: The API provides a portable programming interface to
  cryptographic operations, and key storage functionality on a wide range of
  hardware.

* `PSA Secure Storage API`_: The API provides key/value storage
  interfaces for use with device-protected storage. The Secure Storage API
  describes two interfaces for storage:

    * Internal Trusted Storage (ITS) API: An interface for storage provided by
      the Platform Root of Trust (PRoT).
    * Protected Storage (PS) API: An interface for external protected storage.


.. _design_primary_compute_secure_services:

*******************************
Primary Compute Secure Services
*******************************

On Primary Compute, the implementation of `Crypto Service`_ and `Secure Storage
Service`_ is based on the SE Proxy secure partition.

The Primary Compute also provides the implementation of
`UEFI SMM Services`_ via the SMM Gateway Secure Partition (SMM Gateway SP) to
support UEFI System Management Mode (SMM).

These Secure Services are provided by the `Trusted Services`_ project, and
implemented by leveraging the `TrustZone`_ technology in the Primary Compute and
the hardware-isolated secure enclave in the RSE.

The Reference Software Stack provides the implementation of Secure Services
through both the Primary Compute and the Safety Island.

.. _design_primary_compute_secure_services_architecture:

Architecture
============

The following diagram illustrates the components and data flow that implement
the Primary Compute Secure Services.

|

.. image:: ../images/primary_compute_secure_services.*
   :align: center
   :alt: Primary Compute Secure Services

|

PSA Secure Storage and Crypto APIs Arch Tests
=============================================

The PSA Protected Storage, PSA Internal Trusted Storage and PSA Crypto APIs
Arch Tests can be accessed from the Primary Compute Linux terminal by running
a single command for each. The test suites take around a minute to execute, and
a table of results is displayed upon completion.

Refer to :ref:`validation_trusted_services_tests` for more information.

Parsec
======

`Parsec`_, The Platform AbstRaction for SECurity, is an open-source initiative
to provide a common API to hardware security and cryptographic services in a
platform-agnostic way. This abstraction layer keeps workloads decoupled from
physical platform details.

``Parsec`` is configured to use Trusted Services in the Secure world as its
backend. ``Parsec`` service calls the API provided by ``libts`` which further
invokes the RSE for cryptographic services.

libts
=====

In Linux userspace, the Secure Services are provided in the form of `libts`_
API. ``libts`` is a library that is provided by `Trusted Services`_ for handling
service discovery and Remote Procedure Call (RPC) messaging. ``libts`` entirely
decouples client applications from details of where a service provider is
deployed and how to communicate with it.

The client application sends operation requests and receives responses by
calling the ``libts`` API. ``libts`` communicates with the `Secure Partition`_
(SP) running in the Secure world. The communication between ``libts`` and the
Secure world SP is carried by the `Arm Firmware Framework for Arm A-profile`_
(FF-A) call which is supported by Linux kernel and Trusted Firmware-A.

SE Proxy SP
===========

The Secure Enclave Proxy Secure Partition (`SE Proxy SP`_) is a proxy partition
managed by `OP-TEE`_. It provides access to services hosted by the RSE.

The ``SE Proxy SP`` receives Secure Service operation requests from the Normal
world, translates the request parameters to IPC calls, and invokes the runtime
services provided by the RSE. The IPC is carried by Shared Memory and Message 
Handling Unit (MHUv3) Doorbell communication between the Primary Compute and the
RSE.

SMM Gateway SP
==============

The System Management Mode Gateway Secure Partition (`SMM Gateway SP`_) serves
as a gateway for the variable storage required by the implementation of UEFI
Boot and Runtime Services APIs. These UEFI variables are stored in the Protected
Storage Service provided by the RSE.

The data flow to store UEFI variables is presented in the diagram at the
beginning of the :ref:`design_primary_compute_secure_services_architecture`
section. The U-Boot implementation of the UEFI subsystem uses the FF-A driver to
communicate with the `UEFI SMM Services`_ in the `SMM Gateway SP`_.
The backend of the SMM services uses the Protected Storage proxy from the
`SE Proxy SP`_. From there on, the Protected Storage calls are forwarded to the
secure enclave as explained above.

*****************************
Safety Island Secure Services
*****************************

The Safety Island provides the implementation of `Crypto Service`_ and
`Secure Storage Service`_. The data paths of the services are different.

Architecture
============

The following diagram illustrates the components and data flow that implement
the Safety Island Secure Services.

|

.. image:: ../images/safety_island_secure_services.*
   :align: center
   :alt: Safety Island Secure Services

|

.. _design_safety_island_secure_services_psa_crypto_apis:

PSA Crypto APIs
===============

The `PSA Crypto API`_ is implemented by the ``libmbedcrypto`` library of
`Mbed TLS`_.

Mbed TLS supports drivers for cryptographic accelerators, secure elements and
random number generators. An RSE Communication Driver is created to communicate with
RSE for calling the crypto service that is provided there. The driver invokes
the ``psa_call()`` interface to communicate with the RSE via MHUv3.

By introducing the driver, different crypto operations can be handled in
different ways:

* Asymmetric crypto operations can be handled in the RSE for enhanced security,
  because the private key cannot leave RSE. The following Crypto APIs are
  supported by the driver:

  Key management:
    * ``psa_import_key``
    * ``psa_generate_key``
    * ``psa_copy_key``
    * ``psa_destroy_key``
    * ``psa_export_key``
    * ``psa_export_public_key``

  Asymmetric signature:
    * ``psa_sign_message``
    * ``psa_verify_message``
    * ``psa_sign_hash``
    * ``psa_verify_hash``

  Asymmetric encryption:
    * ``psa_asymmetric_encrypt``
    * ``psa_asymmetric_decrypt``

* Symmetric and other crypto operations are handled in the Safety Island locally
  with the Mbed TLS software implementation, where the runtime performance is
  optimized.

PSA Secure Storage APIs
=======================

Two use cases are addressed by `PSA Secure Storage API`_:

* Internal Trusted Storage:
  Internal Trusted Storage aims at providing a place for devices to store
  their most intimate secrets, either to ensure data privacy or data integrity.
  For example, a device identity key requires confidentiality, whereas an
  authority public key is public data but requires integrity. Other critical
  values that are part of a Root of Trust Service — for example, secure time
  values, monotonic counter values, or firmware image hashes — will also need
  trusted storage.

  The following PSA Internal Trusted Storage APIs are supported in Arm
  Automotive Solutions:

    * ``psa_its_set``
    * ``psa_its_get``
    * ``psa_its_get_info``
    * ``psa_its_remove``

* Protected Storage:
  This is storage which can be used to protect large data-sets against physical
  attacks. It aims to provide the ability for a firmware developer to store
  data onto external flash, with a promise of data-at-rest protection,
  including device-bound encryption, integrity, and replay protection.
  It should be possible to select the appropriate protection level — for
  example, encryption only, or integrity only, or both — depending on the
  threat model of the device and the nature of its deployment.

  The following PSA Protected Storage APIs are supported in Arm Automotive
  Solutions:

    * ``psa_ps_set``
    * ``psa_ps_get``
    * ``psa_ps_get_info``
    * ``psa_ps_remove``
    * ``psa_ps_get_support``

All the PSA Secure Storage API interfaces use the ``psa_call()`` for
communicating with the RSE.

The PSA APIs are thread safe in case of parallel API invocations from multiple
threads within the same cluster or from different clusters, Where the
``psa_call()`` blocks any new requests using a semaphore until the ongoing
request completes.

Memory Map
==========

RSE shares dedicated SRAM with Safety Island Clusters 0, 1, and 2 and Primary
Compute.

Safety Island side:

 Cluster 0:

   * ``local_sram_rse_cl0`` : Used for data transfer between Cluster 0 and RSE

    Refer to the device tree overlay below for more information about the memory
    addresses and region sizes.

     * :repo:`components/safety_island/zephyr/src/overlays/psa/fvp_rd_kronos_safety_island_c0.overlay`.

 Cluster 1:

   * ``local_sram_rse_cl1`` : Used for data transfer between Cluster 1 and RSE

    Refer to the device tree overlay below for more information about the memory
    addresses and region sizes.

     * :repo:`components/safety_island/zephyr/src/overlays/psa/fvp_rd_kronos_safety_island_c1.overlay`.

 Cluster 2:

   * ``local_sram_rse_cl2`` : Used for data transfer between Cluster 2 and RSE

    Refer to the device tree overlay below for more information about the memory
    addresses and region sizes.

     * :repo:`components/safety_island/zephyr/src/overlays/psa/fvp_rd_kronos_safety_island_c2.overlay`.

Primary Compute side:

 * ``RSE_comms-virtio`` : Used for data transfer between SE Proxy SP in the Primary
   Compute Secure World and RSE

RSE communication
=================

The RSE communication protocol is designed to be a lightweight serialization of
the ``psa_call()`` API through a combination of in-band MHUv3
transport and parameter-passing through Shared Memory.

To call an RSE service, the client must send a message in-band over the MHUv3
sender link to RSE and wait for a reply message on the MHUv3 receiver.
The messages are defined as packed C structures, which are serialized in
byte-order over the MHUv3 links.

**********************
Secure Storage Layouts
**********************

The secure storage is handled by the TF-M on the RSE and its configuration 
is based on multiple macros in the codebase. 

The two different types of storage are:
  * Protected Storage
  * Internal Trusted Storage

Protected Storage
=================

The PS is a flash memory area (usually from an off-chip device)
that can be used to store generic data. In the Kronos Reference Design, 
it is used to store EFI variables and recorded FMU faults. PS will 
store each of them as data chunks within an asset.

The maximum number and maximum size of the PS assets are configured in
TF-M configuration. However, TF-M does not validate whether the 
potential total size of assets exceeds the size of the PS area.

Configuration:
The configuration used for TF-M is as follows:

.. list-table:: PS configurations in TF-M
   :widths: 25 25 50
   :header-rows: 1

   * - PS config
     - Value  
     - Description 
   * - PS_MAX_ASSET_SIZE
     - 3456 bytes
     - Maximum size of each asset
   * - PS_NUM_ASSETS
     - 20 Assets
     - Maximum number of assets that can be stored
   * - RSS_FLASH_PS_SIZE
     - 0x10000 (64Kb) 
     - Size in bytes of the protected storage area

.. warning::
    The actual number of the assets stored in PS can vary 
    according to the sizes of the assets. In a rare case, 
    if all the asset sizes are `PS_MAX_ASSET_SIZE`, at most
    only 18 assets can be stored in PS area. Otherwise, PS 
    can run out of storage.


Asset usage in PS
-----------------

EFI Variables
^^^^^^^^^^^^^

An asset can store upto a total of 16 EFI variables, where
each EFI variable takes 216 bytes of storage in the asset.

Here is a memory layout:

.. image:: ../images/ps_efi_asset_layout.*
   :align: center
   :alt: Memory Layout for EFI variable in asset

FMU fault storage
^^^^^^^^^^^^^^^^^
The implementation limits the number of entries stored in the asset to a 
maximum number defined by the Kconfig symbol `MAX_PSA_PROTECTED_STORAGE_SIZE` 
and its default value is 50. Content is serialized into an array of 
`MAX_PSA_PROTECTED_STORAGE_SIZE` struct items, where the struct is formed by 
two 8-byte fields: key and value.

The memory layout looks like following on the asset:

.. image:: ../images/ps_fmu_asset_layout.*
   :align: center
   :alt: Memory Layout for FMU faults in asset
 

Internal Trusted Storage
========================

The ITS is a flash memory area (usually from an on-chip device) that 
can be used to store assets that require high confidentiality. 

Configuration:
The configuration used for TF-M is as follows:

.. list-table:: ITS configurations in TF-M
   :widths: 25 25 50
   :header-rows: 1

   * - ITS config
     - Value  
     - Description 
   * - ITS_MAX_ASSET_SIZE
     - 4096 bytes
     - Maximum size of each asset
   * - ITS_NUM_ASSETS
     - 20 Assets
     - Maximum number of assets that can be stored
   * - RSS_FLASH_ITS_SIZE
     - 0x100000 (1MB) 
     - Size in bytes of the PS area


*******************
RSE Secure Firmware
*******************

The Secure Services are served by the RSE Secure Firmware. For more information
about how the Secure Services work in the RSE, read the `TF-M Secure Services`_
page.

Trusted Firmware-M has some limitations regarding the Secure Storage Service.

Refer to the release notes :ref:`releasenotes_limitations` section for more
details.

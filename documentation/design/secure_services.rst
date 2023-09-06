..
 # SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
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

The Arm Kronos Reference Software Stack provides the implementation of 2 kinds
of secure services defined by following specifications:

* `PSA Cryptography API`_: The API provides a portable programming interface
  to cryptographic operations, and key storage functionality on a wide range of
  hardware.

* `PSA Secure Storage API`_: The API provides key/value storage
  interfaces for use with device-protected storage. The Secure Storage API
  describes two interfaces for storage:

    * Internal Trusted Storage (ITS) API: An interface for storage provided by
      the Platform Root of Trust (PRoT). For now the ITS API is not supported.
    * Protected Storage (PS) API: An interface for external protected storage.

The secure services is implemented by leveraging `TrustZone`_ technology in the
Primary Compute and the RSS, the hardware isolated secure enclave.

************
Architecture
************

The following diagram illustrates the components and data flow that implement
the secure services.

|

.. image:: ../images/secure_services.svg
   :align: center

|

Parsec
======

`Parsec`_, The Platform AbstRaction for SECurity, is an open-source initiative
to provide a common API to hardware security and cryptographic services in a
platform-agnostic way. This abstraction layer keeps workloads decoupled from
physical platform details.

``Parsec`` is configured to use Trusted Services in the secure world as its
backend. ``Parsec`` service calls the API provided by ``libts`` which further
invokes the RSS for cryptographic services.

libts
=====

In Linux userspace, the secure services is provided in the form of `libts`_ API.
``libts`` is a library that is provided by `Trusted Services`_ for handling
service discovery and Remote Procedure Call (RPC) messaging. ``libts`` entirely
decouples client applications from details of where a service provider is
deployed and how to communicate with it.

The client application sends operation requests and receives responses by
calling the ``libts`` API. ``libts`` communicates with the Secure Partition (SP)
running in the secure world. The communication between ``libts`` and the secure
world SP is carried by the `Arm Firmware Framework for Arm A-profile`_ (FF-A)
call which is supported by Linux kernel and Trusted Firmware-A.

SE Proxy SP
===========

The `SE Proxy SP`_ (Secure Enclave Proxy Secure Partition) is a proxy partition
managed by `OP-TEE`_. It provides access to services hosted by the RSS.

The ``SE Proxy SP`` receives secure service operation requests from the
non-secure world, translates the request parameters to IPC calls, and invokes
the runtime services provided by the RSS. The IPC is carried by shared memory
and MHUv3 doorbell communication between the AP and the RSS.

RSS Secure Firmware
===================

The secure services are finally served by the ``RSS Secure Firmware``. For more
information about how the secure services work in the RSS, please read the
`TF-M Secure Services`_ page.

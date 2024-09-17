..
 # SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
 # affiliates <open-source-office@arm.com></text>
 #
 # SPDX-License-Identifier: MIT

########
Overview
########

************
Introduction
************

The Arm Automotive Solutions Software Reference Stack contains a collection
of resources to provide a representative view of typical compute subsystems
that can be designed and implemented using specific generations of Arm
Reference Designs, targeting the automotive sector.

The **Arm Reference Design-1 AE**, or **RD-1 AE** introduces the concept of a
high-performance |Neoverse| V3AE Application Processor (Primary Compute) system
augmented with an |Cortex|-R82AE based Safety Island for scenarios where
additional system safety monitoring is required. The system additionally
includes a Runtime Security Engine (RSE) used for the secure boot of the system
elements and the runtime Secure Services.

Throughout the following documentation, the alias "Kronos Reference Design"
is used in place of Arm Reference Design-1 AE. For more information,
including how to obtain the Technical Overview document, visit the
`Arm Reference Design-1 AE page on developer.arm.com`_.

A Fixed Virtual Platform (FVP) is available as part of the Reference Design.
Further information on FVPs, including expected runtime performance and other
capabilities, can be found at `Arm Ecosystem FVPs`_.

This documentation, together with the RD-1 AE FVP, allow for the exploration of
baremetal and Xen hypervisor hosted Linux instances, Primary Compute to/from
Safety Island communication mechanisms (for both baremetal and virtualized
scenarios), and boot flows coordinated via a system root of trust. The Primary
Compute firmware stack of Trusted Firmware-A, U-Boot, OP-TEE and Trusted
Services is also aligned with the technologies and goals of the
|Arm SystemReadyTM| IR program.

********
Audience
********

The intended target audience of this document are software, hardware, and system
engineers who are planning to evaluate and use Arm Automotive Solutions.

It describes how to build and run images for Arm automotive reference designs
using the Yocto Project build framework. Basic instructions about the Yocto
Project can be found in the `Yocto Project Quick Start`_.

In addition to having Yocto related knowledge, the target audience also needs
to have a certain understanding of the following technologies:

  * Arm Firmware:

    * `OP-TEE`_

    * `Runtime Security Engine (RSE)`_

    * `System Control Processor (SCP) Firmware`_

    * `Trusted Firmware-A (TF-A)`_

    * `Trusted Services`_

  * `U-boot`_

  * `Xen Hypervisor`_

  * `Zephyr`_


Documentation Structure
=======================

  * :ref:`User Guide <user_guide/index:User Guide>`

    Provides guidance for configuring, building, and deploying the solutions
    on the FVP and running and validating the supported functionalities. Also
    provides instructions on additional, user-defined image customization.

  * :ref:`Solution Design <design/index:Solution Design>`

    Provides more advanced developer-focused details of each solution, its
    implementation, and dependencies.

  * :ref:`License <license_link:License>`

    Defines the license under which Arm Automotive Solutions is provided.

  * :ref:`Release Notes <releasenotes:Release Notes>`

    Documents new features, bug fixes, limitations, and any other changes
    provided under each Arm Automotive Solutions release.

.. _overview_reference_software_stack_overview:

*********************************
Arm Automotive Solutions Overview
*********************************

Arm Automotive Solutions is composed of multiple Open Source components,
including:

  * The `Runtime Security Engine (RSE)`_, running an instance of Trusted
    Firmware-M, which offers boot, cryptography, and secure storage services.

  * The Safety Island subsystem, running three instances of the Zephyr real-time
    operating system (RTOS).

  * The firmware for the Primary Compute, using Trusted Firmware-A, U-Boot,
    OP-TEE and Trusted Services. These are configured to be aligned with `Arm
    SystemReady IR`_.

The platform consists of the following hardware IP:

.. table:: Arm Automotive Solutions platform hardware IP
   :widths: auto
   :align: center

   =================== ======================================
   Component           RD-1 AE Reference Design
   =================== ======================================
   Primary Compute     Neoverse-V3AE Armv9.2-A (16 clusters)
   Safety Island       Cortex-R82AE Armv8-R64
   RSE                 Cortex-M55 Armv8.1-M
   SCP                 Cortex-M7 Armv7-M
   =================== ======================================

The remaining software in the Primary Compute subsystem, based on the
`EWAOL`_ distribution, is available in two main architectures:
baremetal and virtualization.

  **Baremetal Architecture**

  The Primary Compute boots a single rich operating system (real-time Linux with
  PREEMPT_RT patches).

.. image:: images/baremetal_high_level_arch.*
   :align: center
   :alt: Arm Automotive Solutions High-Level Diagram - Baremetal Architecture

|

  **Virtualization Architecture**

    The Primary Compute boots into a type-1 hypervisor (Xen) using Arm’s
    hardware virtualization support. There are three isolated, resource-managed
    virtual machines: Dom0 (privileged domain) and DomU1 and DomU2 (unprivileged
    domains).

.. image:: images/virtualization_high_level_arch.*
   :align: center
   :alt: Arm Automotive Solutions High-Level Diagram - Virtualization Architecture

|

**********************************
Safety and Security Considerations
**********************************

Arm Automotive Solutions is a public example software project that tracks and
pulls upstream components, incorporating their respective security fixes
published over time. Arm partners are responsible for ensuring that the
components they use contain all the required security fixes, if and when they
deploy a product derived from Arm reference solutions.

.. _overview_use_cases:

*********
Use-Cases
*********

Arm Automotive Solutions demonstrates how the following features can be
used to enhance the overall functional safety level of a high-performance
compute platform:

  * Critical Application Monitoring
  * High reliability compute subsystem
  * Safety Island Communication
  * Transport Layer Security (TLS) with hardware cryptography support
  * RSE Secure Services providing PSA Secure Storage and Crypto compliant APIs
  * Arm SystemReady IR-aligned software stack
  * Secure firmware update following Arm's Security Firmware Update
    Specification
  * System Fault Handling for increased safety

The :ref:`Reproduce <user_guide/reproduce:Reproduce>` section of the User Guide
contains all the instructions necessary to fetch and build the source as well
as to download the required RD-1 AE FVP and launch the Use-Cases.

Following are the main Use-Cases implemented by the Reference Software Stack.

Critical Application Monitoring Demo
====================================

Critical Application Monitoring (CAM) is a project that implements a solution
for monitoring critical applications using a service running on a higher safety
level system. This demo deploys CAM components on an FVP to demonstrate the
feasibility of the Safety Island monitoring solution.

Refer to :ref:`design_applications_cam` for more information.

Safety Island Actuation Demo
============================

The Safety Island Actuation demo consists of the Arm SystemReady IR-aligned
firmware along with Linux-based software on the Primary Compute and Zephyr
application on the Safety Island to demonstrate automotive workloads.

Refer to :ref:`design_applications_actuation` for more information.

Safety Island Communication Demo
================================

The Safety Island Communication demo demonstrates via Heterogeneous
Inter-processor Communication (HIPC), the networking between:

  * Primary Compute and the three Safety Island clusters.
  * Safety Island clusters.

Refer to :ref:`design_hipc` for more information on HIPC.

Parsec-enabled TLS Demo
=======================

The Parsec-enabled TLS demo illustrates a HTTPS session where a Transport
Layer Security (TLS) connection is established, and a simple webpage is
transferred. The TLS session consists of both symmetric and asymmetric
cryptographic operations. The symmetric operations are executed by Mbed TLS
in Linux userspace on the Primary Compute. The asymmetric operations are
carried out by `Parsec`_, whose backend is based on the RSE cryptographic
runtime service.

Refer to :ref:`design_applications_parsec_enabled_tls` for more information.

Primary Compute PSA Protected Storage and Crypto APIs Architecture Test Suite
=============================================================================

The PSA Protected Storage and Crypto architecture test suites are a set of
examples of the invariant behaviors that are specified in the PSA Protected
Storage APIs and PSA Crypto APIs specifications respectively.

Both suites are used to verify whether these behaviors are implemented
correctly in our system. This suites contain self-checking and portable
C-based tests with a directed stimulus.

Refer to :ref:`design_primary_compute_secure_services` for more information.

Safety Island PSA Secure Storage APIs Architecture Test Suite
=============================================================

The PSA Secure Storage architecture test suite is a set of examples of
the invariant behaviors that are specified in the PSA Secure Storage
APIs specification.

This suite is used to verify whether these behaviors are implemented
correctly in our system. This suite contains self-checking and portable
C-based tests with a directed stimulus.

Refer to :ref:`design_applications_psa_arch_tests_secure_storage` for
more information.

Safety Island PSA Crypto APIs Architecture Test Suite
=====================================================

The PSA Crypto architecture test suite is a set of examples of the invariant
behaviors that are specified in the PSA Crypto APIs specification.

This suite is used to verify whether the PSA Crypto APIs provided on Safety
Island are correctly implemented.

Refer to :ref:`design_applications_psa_arch_tests_crypto` for more
information.

Fault Management Demo
=====================

The Fault Management subsystem for the Safety Island demonstrates the
injection, reporting and collation of faults from supported hardware to support
the design of safety-critical systems.

Refer to :ref:`design_applications_fault_mgmt` for more information.

Arm SystemReady IR Validation
=============================
Arm SystemReady is a compliance certification program based on a set of
hardware and firmware standards that enable interoperability with generic
off-the-shelf operating systems and hypervisors.

Refer to :ref:`design_systemready_ir` for more information.

Linux Distribution Installation
===============================

Demonstrates the installation of three unmodified generic UEFI distribution
images, Debian, openSUSE and Fedora, fulfilling Arm SystemReady requirements.

Secure Firmware Update
======================

Demonstrates an implementation of Secure Firmware Update initiated from
the Primary Compute and follows the
`Platform Security Firmware Update Specification`_.

Refer to :ref:`design_secure_firmware_update` for more information.

********************
Repository Structure
********************

The Arm Automotive Solutions repository (|arm auto solutions repository|) is
structured as follows:

  * ``arm-auto-solutions``:

    * ``yocto``

      Directory implementing the ``meta-arm-auto-solutions`` Yocto layer as well as
      kas build configuration files.

    * ``components``

      Directory containing source code for components which can either be used
      directly or as part of the ``meta-arm-auto-solutions`` Yocto layer.

    * ``documentation``

      Directory which contains the documentation sources, defined in
      ReStructuredText (``.rst``) format for building via ``sphinx``.

******************
Repository License
******************

The repository's standard license is the MIT license (more details in
:ref:`license_link:License`), under which most of the repository's content is
provided. Exceptions to this standard license relate to files that represent
modifications to externally licensed works (for example, patch files). These
files may therefore be included in the repository under alternative licenses in
order to be compliant with the licensing requirements of the associated external
works.

The Arm corporate logo and words marked with ® or ™ are registered trademarks
or trademarks of Arm Limited (or its affiliates) in the US and/or elsewhere.
All rights reserved. Other brands and names mentioned in this document may be
the trademarks of their respective owners.
Please follow Arm’s trademark usage guidelines at `Arm Trademark Policies`_.

*********************************
Contributions and Issue Reporting
*********************************

This project has not put in place a process for contributions currently.

To report issues with the repository such as potential bugs, security concerns,
or feature requests, submit an Issue via `GitLab Issues`_, following the
project's template.

********************
Feedback and Support
********************

To request support contact Arm at support@arm.com. Arm licensees may
also contact Arm via their partner managers.

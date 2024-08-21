..
 # SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
 # affiliates <open-source-office@arm.com></text>
 #
 # SPDX-License-Identifier: MIT

.. _design_systemready_ir:

##################
Arm SystemReady IR
##################

`Arm SystemReady`_ is a compliance certification program based on a set of
hardware and firmware standards that enable interoperability with generic
off-the-shelf operating systems and hypervisors. These standards include the
`Base System Architecture (BSA)`_ and `Base Boot Requirements (BBR)`_
specifications, and market-specific supplements.

In this way, the `Arm SystemReady program`_ provides a formal set of compute
platform definitions to cover a range of systems from the cloud to IoT and edge,
helping software 'just work' seamlessly across an ecosystem of Arm-based
hardware.

Arm SystemReady is divided into a set of bands with a combination of specs
available to suit the different devices and markets. Arm SystemReady IR is
one of these bands.

`Arm SystemReady IR`_ certified platforms implement a minimum set of hardware
and firmware features that an operating system can depend on to deploy the
operating system image. Hence, Arm SystemReady IR ensures the deployment and
maintenance of standard firmware interfaces and targets both custom (Yocto,
OpenWRT, Buildroot) and pre-built (Debian, Fedora, SUSE) Linux distributions.

At a high level, the IR band requires that:

 * Hardware implements the Base System Architecture (BSA)
 * Firmware implements a subset of UEFI as defined in Embedded Base Boot
   Requirements (EBBR)
 * Firmware by default provides a device tree suitable for booting mainline
   Linux
 * Firmware can be updated using UEFI ``UpdateCapsule()``
 * At least three Linux distros must be able to boot, install, and run storage
   medium tests using the UEFI boot flow

Compliant systems must conform to the:

 * `Base System Architecture (BSA)`_ specification
 * `Embedded Base Boot Requirements (EBBR)`_
 * EBBR recipe of the Arm `Base Boot Requirements (BBR)`_ specification
 * `Device Tree specification`_
 * Ethernet port requirements

It is also recommended to conform to the `Security Interface Extension (SIE)`_
certification. If that's not possible, the following `Base Boot Security
Requirements (BBSR)`_ rules are still required:

 * R140_BBSR: Capsule payloads for updating system firmware must be digitally
   signed
 * R150_BBSR: Before updates to system firmware are applied, images must be
   verified using digital signatures

***********************************
Support in Arm Automotive Solutions
***********************************

This Reference Software Stack aims to be aligned with Arm SystemReady IR
version |SystemReady IR ACS version| by implementing most of its requirements.
Given this is a reference design, the software is not being submitted for
formal certification.

The support for running the Architectural Compliance Tests (ACS) is included in
the Reference Software Stack. For more details on how to run it, refer to
the :ref:`user_guide_reproduce_sr_ir_acs` section of this documentation.

The Arm SystemReady scripts used to check the test results skip the identified
non-alignments which are further described in the
:ref:`boot_process_systemready-non_alignments` section below.

.. _boot_process_systemready-non_alignments:

**************************************
Identified Non-Alignments on RD-Kronos
**************************************

The Reference Software Stack is currently known to have the following
non-alignments:

* Reference Software Stack

  * Kronos system does not support populating the list of runtime variables,
    which will lead to "Can't populate EFI variables. No runtime variables will
    be available".
  * Kronos system does not support verifying signed db/dbx update, which will
    lead to "SecureBoot - Verify signed db/dbx update" test failures in SIE.
  * Kronos system does not support querying authenticated variables, which will
    lead to "RT.QueryVariableInfo - Query Auth Variable" test failures in SIE.
  * Kronos system does not support time base authenticated variables, which will
    lead to a "RT.SetVariable" test failure in SIE.

* Devicetree

  * Missing schemas for components which have not yet been or are not
    appropriate to be upstreamed (``arm,rd1-ae``, ``arm,neoverse-v3ae``,
    ``arm,mpam-msc``, ``arm,slc``, ``arm,si-channel``, ``arm,si-rproc``).

* U-Boot

  * Known limitations of EFI implementation which are excluded in the `EBBR
    Specification - UEFI Runtime Services`_.

  * Known limitations of EFI implementation which are noted as 'Explicit
    justification in a future revision of EBBR is pending' by
    `edk2-test-parser`_.

* Model - FVP

  * Platform-specific limitations, which are noted as excluded in the `EBBR
    Specification - Required Platform Specific Elements`_.

  * ``AES``, ``SHA1`` and ``SHA2`` instructions are marked as unavailable in
    the FVP ``ID_AA64ISAR0_EL1``.

* Test environment

  * No text input is available in the test environment for Simple Text Input Ex
    protocol.

* BSA tests

  * Tests are not compatible with certain devices in the RD-Kronos model.

************************
Arm SystemReady IR Tests
************************

.. _systemready_ir_acs_tests:

Arm SystemReady IR ACS Tests
============================

The Arm SystemReady ACS (Architecture Compliance Suite) is a set of tests
that ensure architectural compliance across different implementations and
variants of the architecture. The ACS is delivered as a prebuilt release image.
The image is a bootable live OS image containing a collection of test suites.

The ACS includes 2 optional SIE subsets:

* `SCT for Security Interface Extension`_
* `Linux Boot for Security Interface Extension`_

This Reference Software Stack supports running the SCT subset to cover the
testing on authenticated variables, Secure Boot variables, and Secure Boot image
loading.

The :meta-arm-repo:`meta-arm-systemready/classes/arm-systemready-acs.bbclass`
class in the meta-arm-systemready Yocto layer contains the common logic to
deploy the Arm SystemReady IR ACS version |SystemReady IR ACS version|
pre-built image and set up the testimage environment. It also contains a
testimage "postfunc" called ``acs_logs_handle`` which generates report files
and checks the results.

The script
:meta-arm-repo:`meta-arm-systemready/lib/oeqa/runtime/cases/arm_systemready_ir_acs.py`
in the meta-arm-systemready Yocto layer monitors the ACS tests output from the
bitbake testimage task.

See :meta-arm-repo:`meta-arm-systemready/README.md` for more details.
To run the tests, refer to :ref:`user_guide_reproduce_sr_ir_acs`.

.. _systemready_ir_linux_install:

Linux Distributions Installation Tests
======================================

The Arm SystemReady IR requires that at least three Linux distros must be able
to boot and install using the UEFI boot flow.

Recipes for testing the installation of Linux distributions are provided under
:meta-arm-repo:`meta-arm-systemready/recipes-test/arm-systemready-linux-distros`.
These recipes help to download the installation CD for the Linux distribution
and generate an empty disk as the target disk for the installation.

See :meta-arm-repo:`meta-arm-systemready/README.md` for more details.
To run the tests, refer to :ref:`user_guide_reproduce_arm_systemready_ir_linux`.

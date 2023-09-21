..
 # SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 # affiliates <open-source-office@arm.com></text>
 #
 # SPDX-License-Identifier: MIT

.. _design_systemready_ir:

######################
|Arm SystemReadyTM| IR
######################

`Arm SystemReady`_ is a compliance certification program based on a set of
hardware and firmware standards that enable interoperability with generic
off-the-shelf operating systems and hypervisors. These standards include the
`Base System Architecture (BSA)`_ and `Base Boot Requirements (BBR)`_
specifications, and market-specific supplements.

In this way, the `Arm SystemReady program`_ provides a formal set of compute
platform definitions to cover a range of systems from the cloud to IoT and edge,
helping software 'just work' seamlessly across an ecosystem of Arm-based
hardware.

|Arm SystemReadyTM| is divided into a set of bands with a combination of specs
available to suit the different devices and markets. |Arm SystemReadyTM| IR is
one of these bands.

`Arm SystemReady IR`_ certified platforms implement a minimum set of hardware
and firmware features that an operating system can depend on to deploy the
operating system image. Hence, |Arm SystemReadyTM| IR ensures the deployment and
maintenance of standard firmware interfaces and targets both custom (Yocto,
OpenWRT, Buildroot) and pre-built (Debian, Fedora, SUSE) Linux distributions.

At a high level, the IR band requires that:

 * Firmware implements a subset of UEFI as defined in Embedded Base Boot
   Requirements (EBBR)
 * Firmware by default provides a device tree suitable for booting mainline
   Linux
 * Firmware can be updated using UEFI UpdateCapsule()
 * At least two Linux distros must be able to boot and install using the UEFI
   boot flow

Compliant systems must conform to the:

 * `Base System Architecture (BSA)`_ specification
 * `Embedded Base Boot Requirements (EBBR)`_
 * EBBR recipe of the Arm `Base Boot Requirements (BBR)`_ specification
 * `Device Tree specification`_

********************************
|Arm SystemReadyTM| IR Objective
********************************

This Reference Stack aims to be aligned with |Arm SystemReadyTM| IR version 1.0,
but does not aim to be |Arm SystemReadyTM| IR certified, meaning that neither
formal compliance testing nor validation are performed.

.. _boot_process_systemready-status:

**************
Current Status
**************

This Reference Stack has the testing capability to check for |Arm SystemReadyTM|
alignment. Please refer to :ref:`user_guide_reproduce_arm_systemready_ir_acs` to
see how to run the |Arm SystemReadyTM| IR `ACS`_ tests in this Reference Stack.

The |Arm SystemReadyTM| IR ACS tests of the Reference Stack use a set of
baseline files to detect any changes in regards to the current status of the
|Arm SystemReadyTM| IR alignment of the system. These files can be found under
:meta-arm-repo:`meta-arm-bsp/arm-systemready/acs/baseline/fvp-rd-kronos` and
describe the current status of the Reference Stack. A high-level summary of the
current non-alignments is described in
:ref:`boot_process_systemready-non_alignments`. Please refer to the baseline
files for more detailed information on each individual set of tests.

.. _boot_process_systemready-non_alignments:

*************************
Identified Non-Alignments
*************************

The Reference Stack is currently known to have the following non-alignments:

* Reference stack

    1. Kronos software implementation does not currently support capsule
       updates, so the ``UpdateCapsule()`` method is currently being invoked
       with invalid parameters (``CapsuleCount - 0``).
    2. Kronos system does not have an EFI System Partition, and EFI variables
       are stored in the Protected Storage Service provided by the RSS. However,
       the current FVP version does not provide a reliable reset, and a forced
       restart is used during the ACS test, which will result in 'Failed to
       persist EFI variables', which in turn will cause the BootServicesTest
       case ``BS.ExitBootServices - ConsistencyTestCheckpoint1`` and several
       RuntimeServicesTest ``SetVariable``/``GetVariable`` cases to fail.
    3. U-Boot uses the 'removable storage' method to boot the EFI payload and
       the EFI boot manager is not configured/used.

* U-Boot

    1. Known limitations of EFI implementation which are excluded in the
       `EBBR Specification - UEFI Runtime Services`_.
    2. Known limitations of EFI implementation which are noted as 'Explicit
       justification in a future revision of EBBR is pending' by
       `edk2-test-parser`_.
    3. The ``UpdateCapsule()`` method does not currently support certain
       invocations with invalid parameters.

* Model - FVP

    1. Platform-specific limitations, which are noted as excluded in the
       `EBBR Specification - Required Platform Specific Elements`_.
    2. ``AES``, ``SHA1`` and ``SHA2`` instructions are marked as unavailable in
       the FVP ``ID_AA64ISAR0_EL1``.
    3. The unreliability of the reset function needs a workaround in the test
       methodology, which leads to the failure of several
       ``SetVariable``/``GetVariable`` cases, as described above.

* Test environment

    1. No text input is available in the test environment for Simple Text
       Input Ex protocol.

* BSA tests

    1. Tests are not compatible with certain devices in the RD-Kronos model.

****************************
|Arm SystemReadyTM| IR Tests
****************************

.. _systemready_ir_acs_tests:

|Arm SystemReadyTM| IR ACS Tests
================================

The |Arm SystemReadyTM| ACS (Architecture Compliance Suite) is a set of tests
that ensure architectural compliance across different implementations and
variants of the architecture. The ACS is delivered as a prebuilt release image.
The image is a bootable live OS image containing a collection of test suites.

The :meta-arm-repo:`meta-arm-systemready/classes/arm-systemready-acs.bbclass`
class in the meta-arm-systemready Yocto layer contains the common logic to
deploy the |Arm SystemReadyTM| IR ACS version |SystemReady IR ACS version|
pre-built image and set up the testimage environment. It also contains a
testimage "postfunc" called ``acs_logs_handle`` which generates report files
and analyzes the test results against a baseline.

The script
:meta-arm-repo:`meta-arm-systemready/lib/oeqa/runtime/cases/arm_systemready_ir_acs.py`
in the meta-arm-systemready Yocto layer monitors the ACS tests output from the
bitbake testimage task.

See :meta-arm-repo:`meta-arm-systemready/README.md` for more details.
To run the tests, please refer to
:ref:`user_guide_reproduce_arm_systemready_ir_acs`.

.. _systemready_ir_linux_install:

Linux Distributions Installation Tests
======================================

The |Arm SystemReadyTM| IR requires that at least two Linux distros must be able
to boot and install using the UEFI boot flow.

Recipes for testing the installation of Linux distributions are provided under
:meta-arm-repo:`meta-arm-systemready/recipes-test/arm-systemready-linux-distros`.
These recipes help to download the installation CD for the Linux distribution
and generate an empty disk as the target disk for the installation.

See :meta-arm-repo:`meta-arm-systemready/README.md` for more details.
To run the tests, please refer to
:ref:`user_guide_reproduce_arm_systemready_ir_linux`.

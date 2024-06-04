..
 # SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
 # affiliates <open-source-office@arm.com></text>
 #
 # SPDX-License-Identifier: MIT

.. _design_applications_psa_arch_tests:

#########################################
Safety Island PSA Architecture Test Suite
#########################################

************
Introduction
************

The `PSA Arch Tests`_ test suite is one of a set of resources provided by Arm
that can help organizations develop products that meet the security requirements
of  PSA Certified on Arm-based platforms. The PSA Certified scheme provides
a framework and methodology that helps silicon manufacturers, system software
providers and OEMs to develop more secure products. Arm resources that support
PSA Certified range from threat models, standard architectures that simplify
development and increase portability, and open-source partnerships that provide
ready-to-use software.

The implementation of the PSA APIs Architecture Test Suite contains tests for
PSA APIs specifications. The tests are available as open source.

The architecture test suite abstracts platform-specific information from
the tests.

*******
Diagram
*******

|

.. image:: ../../images/psa_arch_tests.*
   :align: center
   :alt: Safety Island PSA APIs Architecture Test Suite

|

***********
Device Tree
***********

In Zephyr, the device tree overlays the MHUv3 transmission and reception devices and
defines the shared SRAM memory between the Safety Island cluster and RSS.

The Zephyr overlay device tree for FVP the Kronos board is located at
:kronos-repo:`components/safety_island/zephyr/src/overlays/psa`.

.. _design_applications_psa_arch_tests_secure_storage:

***********************************************
PSA Secure Storage APIs Architecture Test Suite
***********************************************

The `PSA Secure Storage APIs Architecture Test Suite`_ runs on Safety Island
Cluster 2 as a Zephyr application. It uses the PSA Secure Storage APIs
interfaces provided by Trusted Firmware-M which communicates with the Secure
Storage Service provided by the Trusted Firmware-M running on RSS using an RSS
communication protocol.

The PSA Secure Storage API tests are linked into the Trusted Firmware-M PSA
Secure Storage APIs binaries and will automatically run. A log similar to the
following should be visible; it is normal for some tests to be skipped but
there should be no failed tests:

.. code-block:: text

  ***** PSA Architecture Test Suite - Version 1.4 *****
  Running.. Storage Suite
  ******************************************
  TEST: 401 | DESCRIPTION: UID not found check | UT: STORAGE
  [Info] Executing tests from non-secure
  [Info] Executing ITS tests
  [Check 1] Call get API for UID 6 which is not set
  [Check 2] Call get_info API for UID 6 which is not set
  [Check 3] Call remove API for UID 6 which is not set
  [Check 4] Call get API for UID 6 which is removed
  [Check 5] Call get_info API for UID 6 which is removed
  [Check 6] Call remove API for UID 6 which is removed
  Set storage for UID 6
  [Check 7] Call get API for different UID 5
  [Check 8] Call get_info API for different UID 5
  [Check 9] Call remove API for different UID 5

  [Info] Executing PS tests
  [Check 1] Call get API for UID 6 which is not set
  [Check 2] Call get_info API for UID 6 which is not set
  [Check 3] Call remove API for UID 6 which is not set
  [Check 4] Call get API for UID 6 which is removed
  [Check 5] Call get_info API for UID 6 which is removed
  [Check 6] Call remove API for UID 6 which is removed
  Set storage for UID 6
  [Check 7] Call get API for different UID 5
  [Check 8] Call get_info API for different UID 5
  [Check 9] Call remove API for different UID 5

  TEST RESULT: PASSED

  ******************************************

  <further tests removed from log for brevity>

  ************ Storage Suite Report **********
  TOTAL TESTS     : 17
  TOTAL PASSED    : 11
  TOTAL SIM ERROR : 0
  TOTAL FAILED    : 0
  TOTAL SKIPPED   : 6
  ******************************************

There are some limitations behind running
``PSA Secure Storage APIs Architecture Test Suite`` on Safety Island Cluster 2
only. Refer to the release notes :ref:`releasenotes_limitations` section.

PSA Secure Storage APIs
=======================

The PSA Secure Storage APIs are provided by the Trusted Firmware-M interfaces
instead of duplicating code in Kronos Reference Software Stack. They are linked
into Zephyr and use the provided ``psa_call()`` in order to communicate with
the RSS to use the Secure Storage Service provided by Trusted Firmware-M.

Refer to `Trusted Firmware-M PSA Protected Storage Interfaces`_ and
`Trusted Firmware-M PSA Internal Trusted Storage Interfaces`_ for more
information.

.. _design_applications_psa_arch_tests_crypto:

***************************************
PSA Crypto APIs Architecture Test Suite
***************************************

The `PSA Crypto APIs Architecture Test Suite`_ is integrated in a Zephyr
application. The application is deployed on all the 3 Safety Island Clusters.

The test suite contains 61 test cases in total. The test cases are executed in
sequence. At the end of the test suite, a log similar to the following should be
visible on all the 3 Safety Island terminals. Normally, no failure should be
seen:

.. code-block:: text

  ************ Crypto Suite Report **********
  TOTAL TESTS     : 61
  TOTAL PASSED    : 61
  TOTAL SIM ERROR : 0
  TOTAL FAILED    : 0
  TOTAL SKIPPED   : 0
  ******************************************

PSA Crypto APIs
===============

The PSA Crypto APIs are implemented by `Mbed TLS`_. In Mbed TLS, different
crypto APIs are handled in different ways. For asymmetric crypto operations, the
RSS secure service is invoked by calling the ``psa_call()`` interface. The other
crypto operations are handled on Safety Island by Mbed TLS software
implementation. For more information on the Mbed TLS implementation,
refer to :ref:`design_safety_island_secure_services_psa_crypto_apis`.

**********
Validation
**********

See :ref:`validation_si_psa_arch_tests`.

******************
Downstream Changes
******************

Patch files can be found at
:kronos-repo:`yocto/meta-arm-auto-solutions/recipes-kernel/zephyr-kernel/files/psa-arch-tests`
to:

* Add PSA Arch Tests as a Zephyr module.
* Move a Secure Storage test to be the final one in the test suite as it causes
  Denial of Service to the Primary Compute.
* Change the key location of asymmetric crypto operation test cases, so the RSS
  secure service can be called.
* Postpone the time-consuming crypto test case for ``psa_generate_key`` to the
  end of the execution sequence.

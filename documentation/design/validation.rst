..
 # Copyright (c) 2023, Arm Limited.
 #
 # SPDX-License-Identifier: MIT

.. _validation:

##########
Validation
##########

.. _validation_run-time_integration_tests:

**************************
Run-Time Integration Tests
**************************

The run-time integration tests are a mechanism for validating the Reference
Stack's core functionalities.

The tests are run on the image using the oeqa test framework. Please refer to
|OEQA FVP|_ for more information on the this framework.

In this section, details on the structure, implementation and debugging of the
tests is given.

OEQA tests in meta-arm
======================

The Compute Elements and Components tested by the framework are detailed below.
The testing scripts can be found in
:meta-arm-repo:`meta-arm/lib/oeqa/runtime/cases/`.

All of the Computing Elements and Components have their terminal output logged
for debugging.

 * LCP
    The script that implements the test is
    :meta-arm-repo:`meta-arm/lib/oeqa/runtime/cases/test_00_lcp.py`.
    The test waits for the LCP to log that it has successfully initialized and
    started all of its internal modules. It also checks whether the LCP has
    logged any errors, in which case the test fails.

 * RSS
    The script that implements the test is
    :meta-arm-repo:`meta-arm/lib/oeqa/runtime/cases/test_00_rss.py`.
    The test waits for the RSS to log that it is releasing the SCP. This is its
    last action as part of the RSS boot process.

 * SCP
    The script that implements the test is
    :meta-arm-repo:`meta-arm/lib/oeqa/runtime/cases/test_00_scp.py`.
    The test waits for the SCP to log that it has successfully initialized and
    started all of its internal modules. It also checks whether the SCP has
    logged any errors, in which case the test fails.

 * Primary Compute
    * BSP
       The entry point to these tests is
       :meta-arm-repo:`meta-arm/lib/oeqa/runtime/cases/test_20_bsp.py`. To find
       out more about the applicable tests, please refer to
       :ref:`design_bsp_tests`.

    * TF-A
       The script that implements the test is
       :meta-arm-repo:`meta-arm/lib/oeqa/runtime/cases/test_00_trusted_firmware_a.py`.
       The test waits for the Primary Compute to log that it is entering the
       normal world as defined in the RSS boot process.

.. _design_bsp_tests:

BSP Tests
=========


The BSP Tests consist of a series of device tests that can be found in
:meta-arm-repo:`meta-arm/lib/oeqa/runtime/cases/test_20_bsp.py`.

* networking
   Checks that the network device and its correct driver are available and
   accessible via the filesystem and that outbound connections work
   (invoking ``wget``).

* rtc
   Checks that the rtc (real-time clock) device and its correct driver are
   available and accessible via the filesystem and verifies that the
   ``hwclock`` command runs successfully.

* smp
   Checks for CPU availability and that basic functionality works, like
   enabling and stopping CPUs and preventing all of them from being
   disabled at the same time.

* virtiorng
   Check that the virtio-rng device is available through the filesystem and
   that it is able to generate random numbers when required.

* watchdog
   Checks that the watchdog device and its correct driver are available and
   accessible via the filesystem.

Integration Tests Implementation
================================

This section gives a high-level description of how the integration testing logic
is implemented.

To enable the integration tests, the |testimage.bbclass|_ is used. This class
supports running automated tests against images. The class handles loading the
tests and starting the image.

The |Writing New Tests|_ section of the Yocto Manual explains how to write new
tests when using the testimage.bbclass. These are placed under
:meta-arm-repo:`meta-arm/lib/oeqa/runtime/cases` and will be selected by the
different machines/configurations by modifying the ``TEST_SUITES`` variable.
For example, the file
:meta-arm-repo:`meta-arm-bsp/conf/machine/fvp-rd-kronos.conf` adds the
``test_10_linuxboot`` test to the ``TEST_SUITES`` variable.

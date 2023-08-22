..
 # SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 # affiliates <open-source-office@arm.com></text>
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
`OEQA FVP`_ for more information on the this framework.

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

    * HIPC
       The scripts that implement the tests are
       :kronos-repo:`yocto/meta-kronos/lib/oeqa/runtime/cases/test_30_hipc.py` and
       :kronos-repo:`yocto/meta-kronos/lib/oeqa/runtime/cases/test_30_hipc_virtualization.py`.
       The tests below are run for each Safety Island cluster for baremetal and
       virtualization architectures. For the virtualization architecture tests
       are run for each Xen guests created.

       * test_ping_cluster
          The test pings the Safety Island from the Primary Compute and vice
          versa and checks that an answer is received (this test depends on
          **test_linux_login**).

       * test_hipc_cluster
          The test verifies Heterogeneous Inter Processor Communication (HIPC)
          between the Safety Island (using ``zperf``) and the Primary Compute
          (using ``iperf``).
          The tested configurations are:

             * The Safety Island as an iperf server (UDP/TCP) and the Primary
               Compute as a client (UDP/TCP).
             * The Safety Island as an iperf client (UDP/TCP) and the Primary
               Compute as a server (UDP/TCP).

          This test depends on **test_ping_cluster**.

       * test_hipc_cluster_cl{M}_cl{N}
          The test verifies Heterogeneous Inter Processor Communication (HIPC)
          between the Safety Island Clusters (using ``zperf``) where M and
          N are the clusters number.
          The tested configurations are:

             * The Safety Island Cluster {M} as an Zperf server (UDP/TCP)
               and the Safety Island Cluster {N} as a Zperf client (UDP/TCP).

          This test depends on **test_ping_cluster**.

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

* cpu_hotplug
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

To enable the integration tests, the `testimage.bbclass`_ is used. This class
supports running automated tests against images. The class handles loading the
tests and starting the image.

The `Writing New Tests`_ section of the Yocto Manual explains how to write new
tests when using the testimage.bbclass. These are placed under
:meta-arm-repo:`meta-arm/lib/oeqa/runtime/cases` and will be selected by the
different machines/configurations by modifying the ``TEST_SUITES`` variable.
For example, the file
:meta-arm-repo:`meta-arm-bsp/conf/machine/fvp-rd-kronos.conf` adds the
``test_10_linuxboot`` test to the ``TEST_SUITES`` variable.

.. _validation_actuation_demo:

Integration Tests validating the Actuation Demo
===============================================

The ``test_player_to_analyzer`` integration test in
:kronos-repo:`yocto/meta-kronos/lib/oeqa/runtime/cases/test_30_actuation.py`
does a full Player to Packet Analyzer functionality test.

This test invokes the Actuation Player that plays a recorded driving scenario
which triggers the Actuation Service to generate Control Commands to be
forwarded to the host via BSD socket. These Control Commands are then captured
by the Packet Analyzer which validates them against a recorded Control Commands
list that is stored in the form of a CSV file.

.. _validation_zephyr_bridge:

Integration Tests validating the Safety Island Cluster 0 Bridge
===============================================================

The ``test_si{N}_bridge_ethernet0`` integration tests in
:kronos-repo:`yocto/meta-kronos/lib/oeqa/runtime/cases/test_30_si0_bridge_ethernet0.py`
verify the connection between the Host and the bridged Safety Island clusters.
The tested configuration is:

 * The Safety Island as an iperf server (TCP) and the Host as a client (TCP).

UDP is not tested because the user networking of the FVP does not provide
port forwarding for UDP traffic.

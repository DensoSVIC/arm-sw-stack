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

The Processing Elements and Components tested by the framework are detailed
below.
The testing scripts can be found in
:meta-arm-repo:`meta-arm/lib/oeqa/runtime/cases/`.

All of the Processing Elements and Components have their terminal output logged
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
    The test firstly waits for the successful programming of the GIC-Multiview
    and the NI-710AE. Then the test waits for the RSS to log that it is releasing
    the SCP. This is its last action as part of the RSS boot process.

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
       Normal world as defined in the RSS boot process.

    * OP-TEE
       The script that implements the test is
       :meta-arm-repo:`meta-arm/lib/oeqa/runtime/cases/test_00_secure_partition.py`.
       The test waits for the Primary Compute to log that OP-TEE loads the
       required SPs (Secure Partitions) and primary CPU switches to Normal world
       boot.

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

.. _design_trusted_services_tests:

Trusted Services Tests
======================

The meta-arm Yocto layer provides Trusted Service OEQA tests which can be used
for automated `Trusted Services Test Executables`_. The script that implements
the test is
:meta-arm-repo:`meta-arm/lib/oeqa/runtime/cases/trusted_services.py`.

Currently, only the following test cases for `psa-api-test` (from
the `PSA Arch Tests`_ project) are supported:

* ts-psa-crypto-api-test
   Used for PSA API conformance testing for `PSA Crypto API`_.

* ts-psa-ps-api-test
   Used for PSA API conformance testing for `PSA Secure Storage API`_.

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

Integration Tests Validating the Safety Island Actuation Demo
=============================================================

The ``test_player_to_analyzer`` integration test in
:kronos-repo:`yocto/meta-kronos/lib/oeqa/runtime/cases/test_30_actuation.py`
does a full Player to Packet Analyzer functionality test.

This test invokes the Actuation Player that plays a recorded driving scenario
which triggers the Actuation Service to generate Control Commands to be
forwarded to the host via BSD socket. These Control Commands are then captured
by the Packet Analyzer which validates them against a recorded Control Commands
list that is stored in the form of a CSV file.

.. _validation_hipc_demo:

Integration Tests Validating the Safety Island Communication Demo
=================================================================

The scripts that implement the tests are
:kronos-repo:`yocto/meta-kronos/lib/oeqa/runtime/cases/test_30_hipc.py` and
:kronos-repo:`yocto/meta-kronos/lib/oeqa/runtime/cases/test_30_hipc_virtualization.py`.
The tests below are run for each Safety Island cluster for Baremetal and
Virtualization Architectures. For the Virtualization Architecture, tests
are run for each Xen guests created.

* test_ping_cluster
   The test pings the Safety Island from the Primary Compute and vice versa and
   checks that an answer is received.

* test_hipc_cluster
   The test verifies Heterogeneous Inter Processor Communication (HIPC) between
   the Safety Island (using ``zperf``) and the Primary Compute (using ``iperf``).
   The tested configurations are:

      * The Safety Island as an iperf server (UDP/TCP) and the Primary
        Compute as a client (UDP/TCP).
      * The Safety Island as an iperf client (UDP/TCP) and the Primary
        Compute as a server (UDP/TCP).

* test_hipc_cluster_cl{M}_cl{N}
   The test verifies Heterogeneous Inter Processor Communication (HIPC) between
   the Safety Island Clusters (using ``zperf``) where M and N are the clusters
   number.
   The tested configurations are:

      * The Safety Island Cluster {M} as an Zperf server (UDP/TCP)
        and the Safety Island Cluster {N} as a Zperf client (UDP/TCP).

.. _validation_gptp:

Integration Tests Validating gPTP
=================================

The scripts that implement the tests are
:kronos-repo:`yocto/meta-kronos/lib/oeqa/runtime/cases/test_30_gptp_base.py` and
:kronos-repo:`yocto/meta-kronos/lib/oeqa/runtime/cases/test_30_gptp.py`.

* test_ptp_linux_services
      The test ensures the ``ptp4l`` services are running.

* test_ptp_si_clients
      The test verifies that the gPTP services running on the Safety Island
      clusters are in the expected client state. It then introduces a fault in
      the system by bringing down the relevant network interfaces on the server
      side and checks that the state machines on the Safety Island clusters are
      not in a client state anymore. The network interfaces are brought back up
      and the test validates that the state machines get back to the expected
      state.

* test_ptp_domu_client
      The test has the same steps as the Safety Island test above, but targeted
      at the DomUs instead of the Safety Island clusters. It is skipped when not
      using the Virtualization Architecture.

.. _validation_zephyr_bridge:

Integration Tests Validating the Safety Island Cluster 0 Bridge
===============================================================

The ``test_si{N}_bridge_ethernet0`` integration tests in
:kronos-repo:`yocto/meta-kronos/lib/oeqa/runtime/cases/test_30_si0_bridge_ethernet0.py`
verify the connection between the Host and the bridged Safety Island clusters.
The tested configuration is:

 * The Safety Island as an iperf server (TCP) and the Host as a client (TCP).

UDP is not tested because the user networking of the FVP does not provide
port forwarding for UDP traffic.

.. _validation_parsec_enabled_tls_demo:

Integration Tests Validating the Parsec-enabled TLS Demo
================================================================

The ``test_parsec_demo`` integration test in
:kronos-repo:`yocto/meta-kronos/lib/oeqa/runtime/cases/test_40_parsec.py`
verifies the functionality of the crypto service provided by Parsec and the
RSS. The test is only enabled when the use case is ``Safety Island Actuation
Demo`` and the "Baremetal Architecture" is selected.

The test invokes a TLS server and client application. The client consumes the
Parsec service for asymmetric crypto operations. Parsec is configured with
Trusted Services as the backend which further invokes the crypto service from
the hardware isolated RSS.

The test is performed under the following configuration:

 * The TLS server operates on the Primary Compute.
 * The TLS client operates within a container that resides on the Primary
   Compute.

.. _validation_xen:

Integration Tests Validating Xen
================================

The ``test_ptestrunner`` integration test in
:kronos-repo:`yocto/meta-kronos/lib/oeqa/runtime/cases/test_40_virtualization.py`
uses ``ptest-runner`` to execute ``01-xendomains.bats`` BATS
(Bash Automated Test System) tests in
:kronos-repo:`yocto/meta-kronos/recipes-test/xen/files/tests/01-xendomains.bats`,

DomUs lifecycle management
--------------------------

The ``01-xendomains.bats`` BATS test verifies DomU lifecycle management,
including status checking, destroy and restart.

MPAM
----

The MPAM cache partitioning functionality is verified by the
``01-xendomains.bats`` BATS tests from the following aspects:

* Verify if the Dom0 Cache Portion Bitmap (CPBM) value is consistent with the
  pre-set value from the Xen command line.
* Verify if the CPBM values for DomU1 and DomU2 are consistent with the
  pre-set value from the Xen guest configuration files.
* Verify if user can modify the domain CPBM values by the ``xl`` sub-commands
  in :ref:`design_components_xen_mpam`.

.. _validation_gicv4_1_demo:

GICv4.1 vLPI/vSGI Direct Injection Demo
---------------------------------------

The ``01-xendomains.bats`` BATS test verifies GICv4.1 feature enablement,
through pre-set keyword capture in Xen and Dom0 Linux boot log.

The ``test_gicv4_1`` integration test in
:kronos-repo:`yocto/meta-kronos/lib/oeqa/runtime/cases/test_40_gicv4_1.py`
verifies the functionality of GICv4.1 vLPI/vSGI direct injection. The test is
only enabled when the use case is ``Safety Island Actuation Demo`` and the
"Virtualization Architecture" is selected.

The test consists of the following aspects and will only be run on DomU1:

  * PCI AHCI SATA disk ``ahci[0000:00:1f.0]`` has already been assigned to
    DomU1 with static PCI passthrough method.
  * Using ``lspci`` command to check if PCI AHCI SATA disk is properly
    probed.
  * Inspecting ``/proc/interrupts`` and reading non-zero MSI-X interrupts
    from ``ahci[0000:00:00.0]`` captured at domain boot-time to validate
    the functionality of GICv4.1 vLPI direct injection.
  * Inspecting ``/proc/interrupts`` and reading non-zero IPI0 interrupts
    captured at domain boot-time to validate the functionality of
    GICv4.1 vSGI direct injection.

The testing of interrupt injection is not currently validated for run time
operations, e.g. file system actions or data transfer.

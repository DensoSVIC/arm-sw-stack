..
 # SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 # affiliates <open-source-office@arm.com></text>
 #
 # SPDX-License-Identifier: MIT

#########
Reproduce
#########

This section of the User Guide describes how to download, configure, build and
execute this Reference Stack.

************
Introduction
************

This Reference Stack uses the `kas menu tool`_ to configure and customize the
different use cases via a set of configuration options provided in the
configuration menu.

.. note::
  All command examples on this page can be copied by clicking the copy button.
  Any console prompts at the start of each line, comments, or empty lines will
  be automatically excluded from the copied text.

.. _user_guide_reproduce_environment_setup:

****************************
Build Host Environment Setup
****************************


System Requirements
===================

    * x86_64 or aarch64 host to build and execute the Kronos FVP
    * Ubuntu 20.04 Linux distribution
    * At least 200GiB of free disk for the download and builds
    * At least 32GiB of RAM memory


Install Dependencies
====================

Please follow the Yocto Project documentation on
`how to install the essential packages`_ required for the build host.

Install the kas tool and its optional dependency (to use the "menu" plugin):

.. code-block:: console
  :substitutions:

  sudo -H pip3 install --upgrade kas==|kas version| && sudo apt install python3-newt

For more details on kas installation, see `kas Dependencies & installation`_.


.. _user_guide_reproduce_download:

********
Download
********

Download the ``kronos`` repository using Git and checkout on the kronos branch,
via:

.. code-block:: shell
  :substitutions:

  # Change the tag or branch to be fetched by replacing the value supplied to
  # the --branch parameter option

  mkdir -p ~/kronos
  cd ~/kronos
  git clone |kronos remote| --branch |kronos version|

.. _user_guide_reproduce_build:

*****
Build
*****

The provided kas configuration menu can be used to build an image for
different Reference Stack types, and to apply different sets of customizable
parameters. Therefore, the following build guidance is provided as a set of
alternatives to target each of the main supported use cases.

To run the configuration menu:

  .. code-block:: console

    kas menu kronos/Kconfig

.. image:: ../images/kronos_reference_stack_build_config.png
   :align: center

|

.. note::
  To build and run any image for the Kronos FVP the user has to accept its
  `EULA`_, which can be done by selecting the corresponding configuration
  option in the build setup.

Full Software Reference Stack
=============================

The Full Software Reference Stack contains the |Arm SystemReadyTM| firmware
along with Linux-based software on the Primary Compute and Zephyr applications
on the Safety Island to demonstrate the use-cases. The Baremetal Architecture
boots Linux directly and the Virtualization Architecture boots Xen with 2
guests.

Baremetal Architecture
----------------------

To build a baremetal image:

1. Select ``Full Software Reference Stack`` from the ``Reference Stack Type``
   menu.
2. Choose ``Baremetal`` from the
   ``Full Software Reference Stack Architecture`` menu.
3. Then choose ``Save & Build``.

Validation tests can be run on the baremetal images.
See :ref:`reproduce_run-time_integration_tests` for more details on running
run-time validation tests.

.. note::
  The Safety Island Actuation Demo is built as part of the default deployment.

Virtualization Architecture
---------------------------

To build a virtualization image:

1. Select ``Full Software Reference Stack`` from the ``Reference Stack Type``
   menu.
2. Choose ``Virtualization`` from the
   ``Full Software Reference Stack Architecture`` menu.
3. Then choose ``Save & Build``.

As with the baremetal guidance above, the Reference Stack virtualization
image can also run validation tests.
See :ref:`reproduce_run-time_integration_tests` for more details on running
run-time validation tests.

.. note::
  The Safety Island Actuation Demo is built as part of the default deployment.

.. _user_guide_reproduce_sr_ir:

|Arm SystemReadyTM| IR Firmware Validation
==========================================

The |Arm SystemReadyTM| IR Firmware Only option just builds the
|Arm SystemReadyTM| IR-aligned firmware. Optionally, additional artifacts can
be built to validate the firmware.

.. image:: ../images/kronos_reference_stack_build_config_sr_ir.png
   :align: center

|

Firmware Build Only
-------------------

To build the |Arm SystemReadyTM| firmware image:

1. Select ``Arm SystemReady IR Reference Stack`` from the
   ``Reference Stack Type`` menu.
2. Choose ``Firmware Build Only`` from the
   ``Arm SystemReady IR Reference Stack`` menu.
3. Then choose ``Save & Build``.

.. _user_guide_reproduce_sr_ir_acs:

Architecture Compliance Suite (ACS) Tests
-----------------------------------------

To build and run the |Arm SystemReadyTM| IR ACS tests:

1. Select ``Arm SystemReady IR Reference Stack`` from the
   ``Reference Stack Type`` menu.
2. Choose ``Architecture Compliance Suite (ACS) Tests`` from the
   ``Arm SystemReady IR Reference Stack`` menu.
3. Then choose ``Save & Build``.

See :ref:`reproduce_arm_systemready_ir_validation` for more details on running
the |Arm SystemReadyTM| IR ACS tests.

.. _user_guide_reproduce_sr_ir_linux_install:

Linux Distros Installation
--------------------------

To build and run the |Arm SystemReadyTM| IR Linux distros installation tests:

1. Select ``Arm SystemReady IR Reference Stack`` from the
   ``Reference Stack Type`` menu.
2. Choose ``Debian Linux Distro Installation`` or
   ``Fedora Linux Distro Installation`` from the
   ``Arm SystemReady IR Reference Stack`` menu.
3. Then choose ``Save & Build``.
4. Run the following command to start the installation:

   .. code-block:: console

      kas shell -c "../layers/meta-arm/scripts/runfvp --verbose --console"

See :ref:`reproduce_arm_systemready_ir_validation` for more details on running
the Linux distros installation tests.

.. _reproduce_run:

***
Run
***

This section describes how to run the Reference Stack on its FVP and connect to
the Primary Compute to manually execute commands and in this way try out its
different functionalities. This can be done for the Baremetal and
Virtualization Architectures.

.. note::
  FVPs, and Fast Models in general, are functionally accurate, meaning that they
  fully execute all instructions correctly, however they are not cycle accurate.
  The main goal of the Reference Stack is to prove functionality only, and
  should not be used for performance analysis.

The Reference Stack running on the Primary Compute can be logged into as
``root`` user without password in the Linux terminal.

Baremetal Architecture
======================

To start the FVP and connect to the Primary Compute terminal (running Linux):

  .. code-block:: console

    kas shell -c "../layers/meta-arm/scripts/runfvp --verbose --console"

The user should wait for the system to boot and for the Linux prompt to appear.

Virtualization Architecture
===========================

To start the FVP and connect to the Primary Compute terminal (running Linux):

  .. code-block:: console

    kas shell -c "../layers/meta-arm/scripts/runfvp --verbose --console"

The user should wait for the system to boot and for the Linux prompt to appear.
On a virtualization image, this will access Dom0. Use the ``xl`` tool to log
in to the DomU1:

  .. code-block:: console

    xl console domu1

This command will provide a console on the DomU1. To exit, one can enter
``Ctrl+]`` (to access the FVP telnet shell), followed by typing ``send esc``
into the telnet shell and pressing ``Enter``. See the `xl documentation`_ for
further details.

Reproducing the Use-Cases
=========================

Safety Island Actuation Demo
----------------------------

Follow the instructions below to reproduce the ``Actuation Demo`` selected as
``Extra Image Features`` from the :ref:`user_guide_reproduce_build` section.
The Safety Island Actuation Demo is listed
in :ref:`design_applications_actuation`. These instructions can be run on both
the Baremetal and Virtualization architectures and an assumption has been made
that the FVP has been launched as indicated under :ref:`reproduce_run`.

.. note::
  When running the ``runfvp`` command, the Safety Island (SI) Cluster 0
  terminal running the Actuation Service is available via the window titled
  **"FVP terminal_uart_si_cluster0"**.

Baremetal Architecture
**********************

1. Run the ``ping`` command from the Primary Compute (running Linux) to verify
   that it can communicate with the Safety Island (running Zephyr):

   .. code-block:: shell

      # On the Primary Compute terminal
      ping 192.168.0.1 -c 10

   The output should look like the following line, repeated 10 times:

   .. code-block:: shell

      64 bytes from 192.168.0.1 seq=0 ttl=64 time=0.151 ms


2. From a different terminal on the build host, start the Packet Analyzer on
   the host where the FVP is running:

   .. code-block:: shell

      cd ~/kronos/
      # Start the Packet Analyzer
      NATIVE_SYSROOT_BIN=build/tmp_baremetal/work/fvp_rd_kronos-poky-linux/baremetal-image/1.0-r0/recipe-sysroot-native/usr/bin
      ${NATIVE_SYSROOT_BIN}/python3-native/python3 ${NATIVE_SYSROOT_BIN}/actuation_packet_analyzer/packet_analyzer/start_analyzer.py -L debug -a localhost -c ./data

   A message similar to the following should appear on the SI Cluster 0:

   .. code-block:: shell

      Actuation Service initialized.
      Accepted tcp connection from the Packet Analyzer: <11>

3. Start the Player on the Primary Compute which replays a recording of a
   driving scenario:

   .. code-block:: shell

      actuation_player -p /usr/share/actuation_player/

   A message similar to the following should appear on the SI Cluster 0:

   .. code-block:: shell

    51572682601: -0.0000 (m/s^2) |  0.0000 (rad)^M
    51597466928: -0.0000 (m/s^2) |  0.0000 (rad)^M
    51622532911: -0.0000 (m/s^2) |  0.0000 (rad)^M
    51647642316: -0.0000 (m/s^2) |  0.0000 (rad)^M
    51672535849: -0.0000 (m/s^2) |  0.0000 (rad)^M
    51697376579: -0.0000 (m/s^2) |  0.0000 (rad)^M
    51722500414: -0.0000 (m/s^2) |  0.0000 (rad)^M
    51747622543: -0.0000 (m/s^2) |  0.0000 (rad)^M
    51772496466: -0.0000 (m/s^2) |  0.0000 (rad)^M
    Thread get_analyzer_handle performing a blocking accept

   A message similar to the following should appear on the host terminal where
   the Packet Analyzer is running:

   .. code-block:: shell

    INFO : analyzer_client.py/_connect_to: Starting analyze, use Ctrl-C to stop the process.
    INFO : analyzer_client.py/_connect_to: Attempting a connect to (localhost : 49152)
    INFO : analyzer_client.py/_connect_to: Successfully connected to (localhost : 49152)
    INFO : analyzer_client.py/run_analyze_on_chain: (1) Analyzer synced with packet chain
    INFO : analyzer_client.py/run_analyze_on_chain: All expected control packets received
    INFO : analyzer_client.py/_log_jitter: Observed Frequency = 21.36147200, Avg Jitter = 0.02624593, Std Deviation:0.06096328
    INFO : analyzer_client.py/run_analyze_on_chain: End of cycle: AnalyzerResult.SUCCESS

    INFO : analyzer_client.py/_tear_conn: Received fin ack from Actuation Service

    Chain ID   Result
    0          AnalyzerResult.SUCCESS

Virtualization Architecture
***************************

1. Enter the DomU1 console using the ``xl`` tool:

   .. code-block:: shell

      xl console domu1

2. Follow the instructions as for the Baremetal Architecture above, with the
   difference of setting the value of ``NATIVE_SYSROOT_BIN`` to the following
   instead:

   .. code-block:: shell

      NATIVE_SYSROOT_BIN=build/tmp_virtualization/work/fvp_rd_kronos-poky-linux/virtualization-image/1.0-r0/recipe-sysroot-native/usr/bin


3. To leave the DomU1 console, type ``Ctrl+]`` and enter ``send esc``.

.. _reproduce_run-time_integration_tests:

********************
Automated Validation
********************

To enable the validation tests, choose ``Run the tests automatically``
from the ``Runtime Validation Setup`` menu, then choose ``Save & Build``.

The following validation tests can be performed on the Reference Stack:

  * System Integration Tests:

    * Baremetal Architecture Stack:

      For the ``Actuation Demo`` selected as ``Extra Image Features``, a similar
      output is printed out. The complete test suit takes around 8 minutes to
      complete.

      .. code-block:: console

        NOTE: Executing Tasks
        2023-06-06 20:11:44 - INFO     - Creating terminal default on terminal_ns_uart0
        2023-06-06 20:11:53 - INFO     - Creating terminal tf-a on terminal_sec_uart
        2023-06-06 20:11:54 - INFO     - Creating terminal scp on terminal_uart_scp
        2023-06-06 20:11:54 - INFO     - Creating terminal lcp on terminal_uart_lcp
        2023-06-06 20:11:54 - INFO     - Creating terminal rss on terminal_rss_uart
        2023-06-06 20:11:54 - INFO     - Creating terminal safety_island_c0 on terminal_uart_si_cluster0
        2023-06-06 20:11:54 - INFO     - Creating terminal safety_island_c1 on terminal_uart_si_cluster1
        2023-06-06 20:11:54 - INFO     - Creating terminal safety_island_c2 on terminal_uart_si_cluster2
        2023-06-06 20:11:54 - INFO     - default: Waiting for login prompt
        2023-06-06 20:19:57 - INFO     - RESULTS:
        2023-06-06 20:19:57 - INFO     - RESULTS - test_10_linuxlogin.LinuxLoginTest.test_linux_login: PASSED (7.39s)
        2023-06-06 20:19:57 - INFO     - RESULTS - test_10_safety_island_c1.SafetyIslandC1Test.test_cluster1: PASSED (0.00s)
        2023-06-06 20:19:57 - INFO     - RESULTS - test_10_safety_island_c2.SafetyIslandC2Test.test_cluster2: PASSED (0.00s)
        2023-06-06 20:19:57 - INFO     - RESULTS - test_30_actuation.ActuationTest.test_analyzer_help: PASSED (0.24s)
        2023-06-06 20:19:57 - INFO     - RESULTS - test_30_actuation.ActuationTest.test_ping: PASSED (17.43s)
        2023-06-06 20:19:57 - INFO     - RESULTS - test_30_actuation.ActuationTest.test_player_to_analyzer: PASSED (92.06s)
        2023-06-06 20:19:57 - INFO     - RESULTS - test_40_parsec.ParsecTest.test_parsec: PASSED (89.35s)
        2023-06-06 20:19:57 - INFO     - RESULTS - test_00_lcp.LcpTest.test_normal_boot: PASSED (0.00s)
        2023-06-06 20:19:57 - INFO     - RESULTS - test_00_rss.RssTest.test_normal_boot: PASSED (0.00s)
        2023-06-06 20:19:57 - INFO     - RESULTS - test_00_scp.ScpTest.test_normal_boot: PASSED (0.00s)
        2023-06-06 20:19:57 - INFO     - RESULTS - test_00_trusted_firmware_a.TrustedFirmwareTest.test_normal_boot: PASSED (0.00s)
        2023-06-06 20:19:57 - INFO     - RESULTS - test_10_linuxboot.LinuxBootTest.test_linux_boot: PASSED (135.16s)
        2023-06-06 20:19:57 - INFO     - RESULTS - test_20_bsp.BspTest.test_cpu_hotplug: PASSED (93.19s)
        2023-06-06 20:19:57 - INFO     - RESULTS - test_20_bsp.BspTest.test_networking: PASSED (17.70s)
        2023-06-06 20:19:57 - INFO     - RESULTS - test_20_bsp.BspTest.test_rtc: PASSED (8.48s)
        2023-06-06 20:19:57 - INFO     - RESULTS - test_20_bsp.BspTest.test_virtiorng: PASSED (8.22s)
        2023-06-06 20:19:57 - INFO     - RESULTS - test_20_bsp.BspTest.test_watchdog: PASSED (5.50s)
        2023-06-06 20:19:57 - INFO     - SUMMARY:
        2023-06-06 20:19:57 - INFO     - baremetal-image () - Ran 17 tests in 474.728s

      For the ``HIPC Validation`` selected as ``Extra Image Features``, a
      similar output is printed out. The complete test suit takes around 15
      minutes to complete.

      .. code-block:: console

        NOTE: Executing Tasks
        2023-06-07 09:49:02 - INFO     - Creating terminal default on terminal_ns_uart0
        2023-06-07 09:49:11 - INFO     - Creating terminal tf-a on terminal_sec_uart
        2023-06-07 09:49:11 - INFO     - Creating terminal scp on terminal_uart_scp
        2023-06-07 09:49:11 - INFO     - Creating terminal lcp on terminal_uart_lcp
        2023-06-07 09:49:11 - INFO     - Creating terminal rss on terminal_rss_uart
        2023-06-07 09:49:11 - INFO     - Creating terminal safety_island_c0 on terminal_uart_si_cluster0
        2023-06-07 09:49:11 - INFO     - Creating terminal safety_island_c1 on terminal_uart_si_cluster1
        2023-06-07 09:49:11 - INFO     - Creating terminal safety_island_c2 on terminal_uart_si_cluster2
        2023-06-07 09:49:12 - INFO     - default: Waiting for login prompt
        2023-06-07 10:04:24 - INFO     - RESULTS:
        2023-06-07 10:04:24 - INFO     - RESULTS - test_10_linuxlogin.LinuxLoginTest.test_linux_login: PASSED (5.16s)
        2023-06-07 10:04:24 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_hipc_cluster0: PASSED (211.84s)
        2023-06-07 10:04:24 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_hipc_cluster1: PASSED (211.80s)
        2023-06-07 10:04:24 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_hipc_cluster2: PASSED (282.60s)
        2023-06-07 10:04:24 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_ping_cluster0: PASSED (21.53s)
        2023-06-07 10:04:24 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_ping_cluster1: PASSED (20.99s)
        2023-06-07 10:04:24 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_ping_cluster2: PASSED (21.27s)
        2023-06-07 10:04:24 - INFO     - RESULTS - test_00_lcp.LcpTest.test_normal_boot: PASSED (0.00s)
        2023-06-07 10:04:24 - INFO     - RESULTS - test_00_rss.RssTest.test_normal_boot: PASSED (0.00s)
        2023-06-07 10:04:24 - INFO     - RESULTS - test_00_scp.ScpTest.test_normal_boot: PASSED (0.00s)
        2023-06-07 10:04:24 - INFO     - RESULTS - test_00_trusted_firmware_a.TrustedFirmwareTest.test_normal_boot: PASSED (0.00s)
        2023-06-07 10:04:24 - INFO     - RESULTS - test_10_linuxboot.LinuxBootTest.test_linux_boot: PASSED (129.07s)
        2023-06-07 10:04:24 - INFO     - SUMMARY:
        2023-06-07 10:04:24 - INFO     - baremetal-image () - Ran 12 tests in 904.258s

    * Virtualization Architecture Stack:

      For the ``Actuation Demo`` selected as ``Extra Image Features``, a similar
      output is printed out. The complete test suit takes around 22 minutes to
      complete.

      .. code-block:: console

        2023-06-06 20:11:46 - INFO     - Creating terminal default on terminal_ns_uart0
        2023-06-06 20:11:55 - INFO     - Creating terminal tf-a on terminal_sec_uart
        2023-06-06 20:11:55 - INFO     - Creating terminal scp on terminal_uart_scp
        2023-06-06 20:11:55 - INFO     - Creating terminal lcp on terminal_uart_lcp
        2023-06-06 20:11:55 - INFO     - Creating terminal rss on terminal_rss_uart
        2023-06-06 20:11:55 - INFO     - Creating terminal safety_island_c0 on terminal_uart_si_cluster0
        2023-06-06 20:11:55 - INFO     - Creating terminal safety_island_c1 on terminal_uart_si_cluster1
        2023-06-06 20:11:55 - INFO     - Creating terminal safety_island_c2 on terminal_uart_si_cluster2
        2023-06-06 20:11:55 - INFO     - default: Waiting for login prompt
        2023-06-06 20:22:59 - INFO     - 'rtc' not tested in DomU
        2023-06-06 20:22:59 - INFO     - 'virtiorng' not tested in DomU
        2023-06-06 20:22:59 - INFO     - 'watchdog' not tested in DomU
        2023-06-06 20:23:12 - INFO     - 'rtc' not tested in DomU
        2023-06-06 20:23:12 - INFO     - 'virtiorng' not tested in DomU
        2023-06-06 20:23:12 - INFO     - 'watchdog' not tested in DomU
        2023-06-06 20:34:09 - INFO     - RESULTS:
        2023-06-06 20:34:09 - INFO     - RESULTS - test_10_linuxlogin.LinuxLoginTest.test_linux_login: PASSED (0.74s)
        2023-06-06 20:34:09 - INFO     - RESULTS - test_10_safety_island_c1.SafetyIslandC1Test.test_cluster1: PASSED (0.00s)
        2023-06-06 20:34:09 - INFO     - RESULTS - test_10_safety_island_c2.SafetyIslandC2Test.test_cluster2: PASSED (0.00s)
        2023-06-06 20:34:09 - INFO     - RESULTS - test_30_actuation.ActuationTest.test_analyzer_help: PASSED (0.24s)
        2023-06-06 20:34:09 - INFO     - RESULTS - test_30_actuation.ActuationTest.test_ping: PASSED (20.09s)
        2023-06-06 20:34:09 - INFO     - RESULTS - test_30_actuation.ActuationTest.test_player_to_analyzer: PASSED (116.22s)
        2023-06-06 20:34:09 - INFO     - RESULTS - test_40_parsec.ParsecTest.test_parsec: PASSED (93.95s)
        2023-06-06 20:34:09 - INFO     - RESULTS - test_40_virtualization.BspTestDomU1.test_cpu_hotplug: PASSED (6.07s)
        2023-06-06 20:34:09 - INFO     - RESULTS - test_40_virtualization.BspTestDomU1.test_networking: PASSED (2.40s)
        2023-06-06 20:34:09 - INFO     - RESULTS - test_40_virtualization.BspTestDomU2.test_cpu_hotplug: PASSED (1.50s)
        2023-06-06 20:34:09 - INFO     - RESULTS - test_40_virtualization.BspTestDomU2.test_networking: PASSED (2.50s)
        2023-06-06 20:34:09 - INFO     - RESULTS - test_40_virtualization.ParsecDomU1Test.test_parsec: PASSED (71.75s)
        2023-06-06 20:34:09 - INFO     - RESULTS - test_40_virtualization.ParsecDomU2Test.test_parsec: PASSED (79.14s)
        2023-06-06 20:34:09 - INFO     - RESULTS - test_40_virtualization.PtestRunnerDom0Test.test_ptestrunner: PASSED (433.47s)
        2023-06-06 20:34:09 - INFO     - RESULTS - test_00_lcp.LcpTest.test_normal_boot: PASSED (0.00s)
        2023-06-06 20:34:09 - INFO     - RESULTS - test_00_rss.RssTest.test_normal_boot: PASSED (0.00s)
        2023-06-06 20:34:09 - INFO     - RESULTS - test_00_scp.ScpTest.test_normal_boot: PASSED (0.00s)
        2023-06-06 20:34:09 - INFO     - RESULTS - test_00_trusted_firmware_a.TrustedFirmwareTest.test_normal_boot: PASSED (0.00s)
        2023-06-06 20:34:09 - INFO     - RESULTS - test_10_linuxboot.LinuxBootTest.test_linux_boot: PASSED (403.12s)
        2023-06-06 20:34:09 - INFO     - RESULTS - test_20_bsp.BspTest.test_cpu_hotplug: PASSED (12.61s)
        2023-06-06 20:34:09 - INFO     - RESULTS - test_20_bsp.BspTest.test_networking: PASSED (9.84s)
        2023-06-06 20:34:09 - INFO     - RESULTS - test_20_bsp.BspTest.test_rtc: PASSED (6.58s)
        2023-06-06 20:34:09 - INFO     - RESULTS - test_20_bsp.BspTest.test_virtiorng: PASSED (6.69s)
        2023-06-06 20:34:09 - INFO     - RESULTS - test_20_bsp.BspTest.test_watchdog: PASSED (4.35s)
        2023-06-06 20:34:09 - INFO     - RESULTS - test_40_virtualization.BspTestDomU1.test_rtc: SKIPPED (0.00s)
        2023-06-06 20:34:09 - INFO     - RESULTS - test_40_virtualization.BspTestDomU1.test_virtiorng: SKIPPED (0.00s)
        2023-06-06 20:34:09 - INFO     - RESULTS - test_40_virtualization.BspTestDomU1.test_watchdog: SKIPPED (0.00s)
        2023-06-06 20:34:09 - INFO     - RESULTS - test_40_virtualization.BspTestDomU2.test_rtc: SKIPPED (0.00s)
        2023-06-06 20:34:09 - INFO     - RESULTS - test_40_virtualization.BspTestDomU2.test_virtiorng: SKIPPED (0.00s)
        2023-06-06 20:34:09 - INFO     - RESULTS - test_40_virtualization.BspTestDomU2.test_watchdog: SKIPPED (0.00s)
        2023-06-06 20:34:09 - INFO     - SUMMARY:
        2023-06-06 20:34:09 - INFO     - virtualization-image () - Ran 30 tests in 1321.219s

      For the ``HIPC Validation`` selected as ``Extra Image Features``, a
      similar output is printed out. The complete test suit takes around 28
      minutes to complete.

      .. code-block:: console

        2023-06-07 09:48:52 - INFO     - Creating terminal default on terminal_ns_uart0
        2023-06-07 09:49:00 - INFO     - Creating terminal tf-a on terminal_sec_uart
        2023-06-07 09:49:01 - INFO     - Creating terminal scp on terminal_uart_scp
        2023-06-07 09:49:01 - INFO     - Creating terminal lcp on terminal_uart_lcp
        2023-06-07 09:49:01 - INFO     - Creating terminal rss on terminal_rss_uart
        2023-06-07 09:49:01 - INFO     - Creating terminal safety_island_c0 on terminal_uart_si_cluster0
        2023-06-07 09:49:01 - INFO     - Creating terminal safety_island_c1 on terminal_uart_si_cluster1
        2023-06-07 09:49:01 - INFO     - Creating terminal safety_island_c2 on terminal_uart_si_cluster2
        2023-06-07 09:49:01 - INFO     - default: Waiting for login prompt
        2023-06-07 10:11:24 - INFO     - HIPC to Cluster 0 not tested for DomU2
        2023-06-07 10:16:12 - INFO     - HIPC to Cluster 2 not tested for DomU2
        2023-06-07 10:16:12 - INFO     - Ping to Cluster 0 not tested for DomU2
        2023-06-07 10:16:12 - INFO     - Ping to Cluster 2 not tested for DomU2
        2023-06-07 10:16:30 - INFO     - RESULTS:
        2023-06-07 10:16:30 - INFO     - RESULTS - test_10_linuxlogin.LinuxLoginTest.test_linux_login: PASSED (0.79s)
        2023-06-07 10:16:30 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_hipc_cluster0: PASSED (277.58s)
        2023-06-07 10:16:30 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_hipc_cluster1: PASSED (274.31s)
        2023-06-07 10:16:30 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_hipc_cluster2: PASSED (301.45s)
        2023-06-07 10:16:30 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_ping_cluster0: PASSED (28.39s)
        2023-06-07 10:16:30 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_ping_cluster1: PASSED (30.31s)
        2023-06-07 10:16:30 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_ping_cluster2: PASSED (30.89s)
        2023-06-07 10:16:30 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU2.test_hipc_cluster1: PASSED (259.74s)
        2023-06-07 10:16:30 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU2.test_ping_cluster1: PASSED (28.48s)
        2023-06-07 10:16:30 - INFO     - RESULTS - test_00_lcp.LcpTest.test_normal_boot: PASSED (0.00s)
        2023-06-07 10:16:30 - INFO     - RESULTS - test_00_rss.RssTest.test_normal_boot: PASSED (0.00s)
        2023-06-07 10:16:30 - INFO     - RESULTS - test_00_scp.ScpTest.test_normal_boot: PASSED (0.00s)
        2023-06-07 10:16:30 - INFO     - RESULTS - test_00_trusted_firmware_a.TrustedFirmwareTest.test_normal_boot: PASSED (0.00s)
        2023-06-07 10:16:30 - INFO     - RESULTS - test_10_linuxboot.LinuxBootTest.test_linux_boot: PASSED (384.36s)
        2023-06-07 10:16:30 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU2.test_hipc_cluster0: SKIPPED (0.00s)
        2023-06-07 10:16:30 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU2.test_hipc_cluster2: SKIPPED (0.00s)
        2023-06-07 10:16:30 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU2.test_ping_cluster0: SKIPPED (0.00s)
        2023-06-07 10:16:30 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU2.test_ping_cluster2: SKIPPED (0.00s)
        2023-06-07 10:16:30 - INFO     - SUMMARY:
        2023-06-07 10:16:30 - INFO     - virtualization-image () - Ran 18 tests in 1635.156s
        2023-06-07 10:16:30 - INFO     - virtualization-image - OK - All required tests passed (successes=14, skipped=4, failures=0, errors=0)

.. _reproduce_arm_systemready_ir_validation:

*********************************
|Arm SystemReadyTM| IR Validation
*********************************

|Arm SystemReadyTM| IR ACS Tests
================================

The ACS for the |Arm SystemReadyTM| IR certification is delivered through a live
OS image, which enables the basic automation to run the tests.

Follow the steps listed in :ref:`user_guide_reproduce_sr_ir_acs`, the system
will boot with the ACS live OS image and the ACS tests will run automatically
after the system boots.

The previous tests take around 8 hours to complete. A similar output should be
printed out:

.. code-block:: console

  2023-05-16 03:50:16 - INFO     - NOTE: recipe arm-systemready-ir-acs-1.0-r0: task do_testimage: Started
  2023-05-16 03:50:16 - INFO     - Creating terminal default on terminal_ns_uart0
  2023-05-16 03:50:25 - INFO     - Creating terminal tf-a on terminal_sec_uart
  2023-05-16 03:50:25 - INFO     - Creating terminal scp on terminal_uart_scp
  2023-05-16 03:50:25 - INFO     - Creating terminal lcp on terminal_uart_lcp
  2023-05-16 03:50:26 - INFO     - Creating terminal rss on terminal_rss_uart
  2023-05-16 03:50:26 - INFO     - Creating terminal safety_island_c0 on terminal_uart_si_cluster0
  2023-05-16 03:50:26 - INFO     - Creating terminal safety_island_c1 on terminal_uart_si_cluster1
  2023-05-16 03:50:26 - INFO     - Creating terminal safety_island_c2 on terminal_uart_si_cluster2
  2023-05-16 03:55:48 - INFO     - Test Group (PlatformSpecificElements): FAILED
  2023-05-16 03:56:45 - INFO     - Test Group (RequiredElements): FAILED
  2023-05-16 03:57:41 - INFO     - Test Group (CheckEvent_Conf): PASSED
  2023-05-16 03:58:37 - INFO     - Test Group (CheckEvent_Func): PASSED
  2023-05-16 03:59:34 - INFO     - Test Group (CloseEvent_Func): PASSED
  2023-05-16 04:00:34 - INFO     - Test Group (CreateEventEx_Conf): PASSED
  2023-05-16 04:01:30 - INFO     - Test Group (CreateEventEx_Func): PASSED
  2023-05-16 04:02:29 - INFO     - Test Group (CreateEvent_Conf): PASSED
  2023-05-16 04:03:26 - INFO     - Test Group (CreateEvent_Func): PASSED
  2023-05-16 04:04:23 - INFO     - Test Group (RaiseTPL_Func): PASSED
  2023-05-16 04:05:19 - INFO     - Test Group (RestoreTPL_Func): PASSED
  2023-05-16 04:06:16 - INFO     - Test Group (SetTimer_Conf): PASSED
  2023-05-16 04:11:54 - INFO     - Test Group (SetTimer_Func): PASSED
  2023-05-16 04:12:51 - INFO     - Test Group (SignalEvent_Func): PASSED
  2023-05-16 04:13:48 - INFO     - Test Group (WaitForEvent_Conf): PASSED
  2023-05-16 04:14:59 - INFO     - Test Group (WaitForEvent_Func): PASSED
  2023-05-16 04:15:56 - INFO     - Test Group (AllocatePages_Conf): PASSED
  2023-05-16 04:18:06 - INFO     - Test Group (AllocatePages_Func): PASSED
  2023-05-16 04:19:02 - INFO     - Test Group (AllocatePool_Conf): PASSED
  2023-05-16 04:20:01 - INFO     - Test Group (AllocatePool_Func): PASSED
  2023-05-16 04:20:57 - INFO     - Test Group (FreePages_Conf): PASSED
  2023-05-16 04:21:56 - INFO     - Test Group (FreePages_Func): PASSED
  2023-05-16 04:22:52 - INFO     - Test Group (GetMemoryMap_Conf): PASSED
  2023-05-16 04:23:48 - INFO     - Test Group (GetMemoryMap_Func): PASSED
  ...
  ...
  2023-05-16 11:18:55 - INFO     - Test Group (virtio_blk virtio1): vda
  2023-05-16 11:19:09 - INFO     - Linux tests complete
  2023-05-16 11:19:18 - INFO     - RESULTS:
  2023-05-16 11:19:18 - INFO     - RESULTS - arm_systemready_ir_acs.SystemReadyACSTest.test_acs: PASSED (26923.49s)
  2023-05-16 11:19:18 - INFO     - SUMMARY:
  2023-05-16 11:19:18 - INFO     - arm-systemready-ir-acs () - Ran 1 test in 26923.488s
  2023-05-16 11:19:18 - INFO     - arm-systemready-ir-acs - OK - All required tests passed (successes=1, skipped=0, failures=0, errors=0)
  2023-05-16 11:19:20 - INFO     - ACS test suite results are consistent with baseline.

Please refer to :ref:`systemready_ir_acs_tests` for an explanation on how the
ACS tests are set up and how they work in the Reference Stack.

Linux Distributions Installation Tests
======================================

The |Arm SystemReadyTM| IR must boot at least two unmodified generic UEFI
distribution images from an ISO image. To test the installation of a Linux
distribution, follow the steps listed in
:ref:`user_guide_reproduce_sr_ir_linux_install` to start the installation.

This Software Stack currently supports two Linux distributions: `Debian Stable`_
and `Fedora Server`_. To install Debian, you can refer to the
`Debian GNU/Linux Installation Guide`_. Similarly, you can refer to the
`Fedora Installation Guide`_ for the installation of Fedora.

.. note::

  The installation of a Linux distribution requires some manual interaction, for
  example, some necessary selections or confirmations, entering the user and
  password, etc.

  The whole installation process takes a long time (possibly up to 24 hours, or
  even longer).

  We suggest that when running the Linux distribution installations the FVP is
  the only running process as it will consume large amounts of RAM that can make
  the system unstable.

Please refer to :ref:`systemready_ir_linux_install` for an explanation on how
the Linux distros installation is set up and how they work in the Reference
Stack.

Below are some tips and possible problems encountered during the installation
process for reference.

Debian
------

The whole process of installing Debian will probably take about 5 hours.

The following are problems that have been encountered during the Debian
installation process and how to solve them:

* Detect and mount installation media

    1. After the installer starts, it will prompt
       ``No device for installation media was detected.`` in the
       ``Detect and mount installation media`` tab.
       Choose ``No`` to continue.

    .. image:: ../images/sr-ir-linux-distro-debian-install-media-0.png
       :align: center

|

    2. Choose ``Yes`` to Manually select a module and device for installation
       media.

    .. image:: ../images/sr-ir-linux-distro-debian-install-media-1.png
       :align: center

|

    3. Choose ``none`` to continue.

    .. image:: ../images/sr-ir-linux-distro-debian-install-media-2.png
       :align: center

|

    4. Input ``/dev/mmcblk0`` as the device file for accessing the installation
       media.

    .. image:: ../images/sr-ir-linux-distro-debian-install-media-3.png
       :align: center

|

* Install the GRUB boot loader

  When the installation reaches the ``Install the GRUB boot loader`` phase,
  there will be an error ``Unable to install GRUB in dummy``.
  This is because on EBBR platform, UEFI SetVariable() is not required at
  runtime (however, it is required at boot time), and Kronos happens to not
  support UEFI SetVariable() yet.

    .. image:: ../images/sr-ir-linux-distro-debian-install-grub.png
       :align: center

|

  One workaround we have is to "execute a shell" when the GRUB install phase
  throws the above error. To execute a shell, press ``Ctrl-a n`` to switch the
  debug shell, and run the following commands:

  .. code-block:: console

     # chroot /target
     # update-grub
     # mkdir /boot/efi/EFI/BOOT
     # cp -v /boot/efi/EFI/debian/grubaa64.efi /boot/efi/EFI/BOOT/bootaa64.efi

  A snapshot is as below:

  .. code-block:: console

     [           1- installer   (2*shell)  3 shell  4 log           ][ Jun 06 23:13 ]
     #
     # chroot /target
     # update-grub
     Generating grub configuration file ...
     Found linux image: /boot/vmlinuz-5.10.0-23-arm64
     Found initrd image: /boot/initrd.img-5.10.0-23-arm64
     Found linux image: /boot/vmlinuz-5.10.0-22-arm64
     Found initrd image: /boot/initrd.img-5.10.0-22-arm64
     Warning: os-prober will be executed to detect other bootable partitions.
     Its output will be used to detect bootable binaries on them and create new boot
     done
     # ls /boot/efi/EFI/debian/
     BOOTAA64.CSV  fbaa64.efi  grub.cfg  grubaa64.efi  mmaa64.efi  shimaa64.efi
     # mkdir /boot/efi/EFI/BOOT
     # cp -v /boot/efi/EFI/debian/grubaa64.efi /boot/efi/EFI/BOOT/bootaa64.efi
     '/boot/efi/EFI/debian/grubaa64.efi' -> '/boot/efi/EFI/BOOT/bootaa64.efi'
     #

Fedora
------

Here are some tips for installing Fedora:

1. It needs a little long time to wait GRUB to load installer kernel and initrd.
2. Choose text mode installer.
3. Use default storage partition setting.
4. The installer will be stuck at "Configuring kernel-core.aarch64" for a long
   time.
5. Wait serval hours, the installer will verify the installed packages and
   continue to install bootloader.
6. The following error occurred while installing the boot loader. Ignore the
   error by responding 'yes' and continue.

   .. code-block:: console

      Installing boot loader
      ================================================================================
      ================================================================================
      Question

      The following error occurred while installing the boot loader. The system will
      not be bootable. Would you like to ignore this and continue with installation?

      Failed to set new efi boot target. This is most likely a kernel or firmware bug.

      Please respond 'yes' or 'no': yes

      [anaconda]1:main* 2:shell  3:log  4:storage-log >Switch tab: Alt+Tab | Help: F1

7. It may need more then 24 hours to complete the installation.
8. Force restart the FVP, and boot the installed OS.
9. Users can login the Linux shell about 20 minutes after restart.

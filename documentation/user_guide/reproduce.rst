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


Install Dependencies
====================

Please follow the Yocto Project documentation on
`how to install the essential packages`_ required for the build host.

Install the kas tool:

.. code-block:: console
  :substitutions:

  sudo -H pip3 install --upgrade kas==|kas version|

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
different system architectures, and to apply different sets of customizable
parameters. Therefore, the following build guidance is provided as a set of
alternatives to target each of the main supported use cases.

To run the configuration menu:

  .. code-block:: console

    kas menu kronos/Kconfig

|

.. image:: ../images/kronos_reference_stack_build_config.png
   :align: center

|

.. note::
  To build and run any image for the Kronos FVP the user has to accept its
  EULA_, which can be done by selecting the corresponding configuration
  option in the build setup.
 

Baremetal Architecture
======================

To build a baremetal image choose ``Baremetal`` from
the ``Reference Stack Architecture`` menu, then choose ``Save & Build``.

Validation tests can be run on the baremetal images.
See :ref:`reproduce_run-time_integration_tests` for more details on running
run-time validation tests.

Virtualization Architecture
===========================

To build a virtualization image choose ``Virtualization`` from
the ``Reference Stack Architecture`` menu, then choose ``Save & Build``.

As with the baremetal guidance above, the Reference Stack virtualization
image can also run validation tests.
See :ref:`reproduce_run-time_integration_tests` for more details on running
run-time validation tests.

Arm SystemReady Firmware Architecture
=====================================

To build an Arm SystemReady Firmware image choose ``Arm SystemReady Firmware``
from the ``Reference Stack Architecture`` menu, then choose ``Save & Build``.


Arm SystemReady IR ACS
======================

To build an Arm SystemReady IR ACS image choose ``Arm SystemReady IR ACS``
from the ``Reference Stack Architecture`` menu, then choose ``Save & Build``.

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

.. _reproduce_run-time_integration_tests:

**********
Validation
**********

To enable the validation tests, choose ``Run the tests automatically``
from the ``Runtime Validation Setup`` menu, then choose ``Save & Build``.

The following validation tests can be performed on the Reference Stack:

  * System Integration Tests:

    * Baremetal Architecture Stack:

      The previous test takes around 10 minutes to complete.

      A similar output should be printed out:

      .. code-block:: console

        NOTE: Executing Tasks
        2022-12-07 09:05:58 - INFO     - Creating terminal default on terminal_ns_uart_ap
        2022-12-07 09:05:58 - INFO     - Creating terminal tf-a on terminal_s_uart_ap
        2022-12-07 09:05:58 - INFO     - Creating terminal scp on terminal_uart_scp
        2022-12-07 09:05:58 - INFO     - Creating terminal mcp on terminal_uart_mcp
        2022-12-07 09:05:58 - INFO     - Creating terminal lcp on terminal_uart_lcp
        2022-12-07 09:05:59 - INFO     - Creating terminal rss on terminal_uart_rss
        2022-12-07 09:05:59 - INFO     - default: Waiting for login prompt
        2022-12-07 09:06:07 - INFO     - RESULTS:
        2022-12-07 09:06:07 - INFO     - RESULTS - test_00_lcp.LcpTest.test_normal_boot: PASSED (8.00s)
        2022-12-07 09:06:07 - INFO     - RESULTS - test_00_rss.RssTest.test_normal_boot: PASSED (0.00s)
        2022-12-07 09:06:07 - INFO     - RESULTS - test_00_scp.ScpTest.test_normal_boot: PASSED (0.00s)
        2022-12-07 09:06:07 - INFO     - RESULTS - test_00_trusted_firmware_a.TrustedFirmwareTest.test_normal_boot: PASSED (0.00s)
        2022-12-07 09:10:02 - INFO     - RESULTS - test_10_linuxboot.LinuxBootTest.test_linux_boot: PASSED (201.16s)
        2022-12-07 09:10:16 - INFO     - RESULTS - test_20_bsp.BspTest.test_networking: PASSED (13.54s)
        2022-12-07 09:11:20 - INFO     - RESULTS - test_20_bsp.BspTest.test_rtc: PASSED (4.61s)
        2022-12-07 09:11:56 - INFO     - RESULTS - test_20_bsp.BspTest.test_cpu_hotplug: PASSED (35.33s)
        2022-12-07 09:12:01 - INFO     - RESULTS - test_20_bsp.BspTest.test_virtiorng: PASSED (5.58s)
        2022-12-07 09:12:04 - INFO     - RESULTS - test_20_bsp.BspTest.test_watchdog: PASSED (3.45s)
        2022-12-07 09:12:04 - INFO     - SUMMARY:
        2022-12-07 09:12:04 - INFO     - baremetal-image () - Ran 10 tests in 271.671s
        2022-12-07 09:12:04 - INFO     - baremetal-image - OK - All required tests passed (successes=10, skipped=0, failures=0, errors=0)

    * Virtualization Architecture Stack:

      The previous test takes around 20 minutes to complete.

      A similar output should be printed out:

      .. code-block:: console

        NOTE: Executing Tasks
        2023-04-12 09:09:10 - INFO     - Creating terminal default on terminal_ns_uart0
        2023-04-12 09:09:18 - INFO     - Creating terminal tf-a on terminal_sec_uart
        2023-04-12 09:09:19 - INFO     - Creating terminal scp on terminal_uart_scp
        2023-04-12 09:09:19 - INFO     - Creating terminal lcp on terminal_uart_lcp
        2023-04-12 09:09:19 - INFO     - Creating terminal rss on terminal_rss_uart
        2023-04-12 09:09:19 - INFO     - Creating terminal safety_island_c0 on terminal_uart_si_cluster0
        2023-04-12 09:09:19 - INFO     - Creating terminal safety_island_c1 on terminal_uart_si_cluster1
        2023-04-12 09:09:19 - INFO     - Creating terminal safety_island_c2 on terminal_uart_si_cluster2
        2023-04-12 09:09:19 - INFO     - default: Waiting for login prompt
        2023-04-12 09:19:19 - INFO     - Bitbake still alive (no events for 600s). Active tasks:
        2023-04-12 09:19:19 - INFO     - /builds/engineering/ais/kronos/kronos/work/build/../../yocto/meta-kronos/dynamic-layers/virtualization-layer/recipes-core/images/virtualization-image.bb:do_testimage
        2023-04-12 09:21:55 - INFO     - 'rtc' not tested in DomU
        2023-04-12 09:21:55 - INFO     - 'virtiorng' not tested in DomU
        2023-04-12 09:21:55 - INFO     - 'watchdog' not tested in DomU
        2023-04-12 09:22:13 - INFO     - 'rtc' not tested in DomU
        2023-04-12 09:22:13 - INFO     - 'virtiorng' not tested in DomU
        2023-04-12 09:22:13 - INFO     - 'watchdog' not tested in DomU
        2023-04-12 09:32:11 - INFO     - RESULTS:
        2023-04-12 09:32:11 - INFO     - RESULTS - test_10_linuxlogin.LinuxLoginTest.test_linux_login: PASSED (0.86s)
        2023-04-12 09:32:11 - INFO     - RESULTS - test_10_safety_island_c0.SafetyIslandC0Test.test_cluster0: PASSED (0.00s)
        2023-04-12 09:32:11 - INFO     - RESULTS - test_10_safety_island_c1.SafetyIslandC1Test.test_cluster1: PASSED (0.00s)
        2023-04-12 09:32:11 - INFO     - RESULTS - test_10_safety_island_c2.SafetyIslandC2Test.test_cluster2: PASSED (0.00s)
        2023-04-12 09:32:11 - INFO     - RESULTS - test_40_virtualization.BspTestDomU1.test_cpu_hotplug: PASSED (17.78s)
        2023-04-12 09:32:11 - INFO     - RESULTS - test_40_virtualization.BspTestDomU1.test_networking: PASSED (2.02s)
        2023-04-12 09:32:11 - INFO     - RESULTS - test_40_virtualization.BspTestDomU2.test_cpu_hotplug: PASSED (3.49s)
        2023-04-12 09:32:11 - INFO     - RESULTS - test_40_virtualization.BspTestDomU2.test_networking: PASSED (3.41s)
        2023-04-12 09:32:11 - INFO     - RESULTS - test_40_virtualization.PtestRunnerDom0Test.test_ptestrunner: PASSED (502.28s)
        2023-04-12 09:32:11 - INFO     - RESULTS - test_00_lcp.LcpTest.test_normal_boot: PASSED (0.00s)
        2023-04-12 09:32:11 - INFO     - RESULTS - test_00_rss.RssTest.test_normal_boot: PASSED (0.00s)
        2023-04-12 09:32:11 - INFO     - RESULTS - test_00_scp.ScpTest.test_normal_boot: PASSED (0.00s)
        2023-04-12 09:32:11 - INFO     - RESULTS - test_00_trusted_firmware_a.TrustedFirmwareTest.test_normal_boot: PASSED (0.00s)
        2023-04-12 09:32:11 - INFO     - RESULTS - test_10_linuxboot.LinuxBootTest.test_linux_boot: PASSED (729.36s)
        2023-04-12 09:32:11 - INFO     - RESULTS - test_20_bsp.BspTest.test_cpu_hotplug: PASSED (26.93s)
        2023-04-12 09:32:11 - INFO     - RESULTS - test_20_bsp.BspTest.test_networking: PASSED (20.97s)
        2023-04-12 09:32:11 - INFO     - RESULTS - test_20_bsp.BspTest.test_rtc: PASSED (11.08s)
        2023-04-12 09:32:11 - INFO     - RESULTS - test_20_bsp.BspTest.test_virtiorng: PASSED (12.24s)
        2023-04-12 09:32:11 - INFO     - RESULTS - test_20_bsp.BspTest.test_watchdog: PASSED (6.87s)
        2023-04-12 09:32:11 - INFO     - RESULTS - test_40_virtualization.BspTestDomU1.test_rtc: SKIPPED (0.00s)
        2023-04-12 09:32:11 - INFO     - RESULTS - test_40_virtualization.BspTestDomU1.test_virtiorng: SKIPPED (0.00s)
        2023-04-12 09:32:11 - INFO     - RESULTS - test_40_virtualization.BspTestDomU1.test_watchdog: SKIPPED (0.00s)
        2023-04-12 09:32:11 - INFO     - RESULTS - test_40_virtualization.BspTestDomU2.test_rtc: SKIPPED (0.00s)
        2023-04-12 09:32:11 - INFO     - RESULTS - test_40_virtualization.BspTestDomU2.test_virtiorng: SKIPPED (0.00s)
        2023-04-12 09:32:11 - INFO     - RESULTS - test_40_virtualization.BspTestDomU2.test_watchdog: SKIPPED (0.00s)
        2023-04-12 09:32:11 - INFO     - SUMMARY:
        2023-04-12 09:32:11 - INFO     - virtualization-image () - Ran 25 tests in 1358.820s
        2023-04-12 09:32:11 - INFO     - virtualization-image - OK - All required tests passed (successes=19, skipped=6, failures=0, errors=0)

  Please refer to :ref:`validation` for an explanation on how the validation
  tests are set up and how they work in the Reference Stack.

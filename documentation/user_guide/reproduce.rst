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
different :ref:`Use-Cases <introduction_use_cases>` via a set of configuration
options provided in the configuration menu.

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
  * Ubuntu Desktop or Server 20.04 Linux distribution
  * At least 300GiB of free disk for the download and builds
  * At least 32GiB of RAM memory
  * At least 8GiB of swap memory


Install Dependencies
====================

  * Please follow the Yocto Project documentation on
    `how to install the essential packages`_ required for the build host. The
    packages needed to build the Yocto Project documentation manuals are not
    required.

  * Install the kas tool and its optional dependency (to use the "menu" plugin):

    .. code-block:: console
      :substitutions:

      sudo -H pip3 install --upgrade kas==|kas version| && sudo apt install python3-newt

    For more details on kas installation, see
    `kas Dependencies & installation`_.
  * Install tmux (required for ``runfvp`` tool):

    .. code-block:: console

      sudo apt install tmux

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
  tmux new-session -s kronos
  git clone |kronos remote| --branch |kronos version|

.. note::
   Performing the builds and FVP execution in a tmux session is mandatory for
   Kronos because the ``runfvp`` tool that invokes the Kronos FVP expects the
   presence of a tmux session to attach its spawned tmux windows for console
   access to the processing elements. Please refer to
   `Tmux Documentation`_ for more information on the usage of tmux. It is
   recommended to change the default ``history-limit`` by adding
   ``set-option -g history-limit 3000`` to ``~/.tmux.conf`` before starting
   tmux.

*************************
Reproducing the Use-Cases
*************************

General
=======

Kas Build
---------

The Kronos stack has a kas configuration menu that can be used to build the
:ref:`introduction_use_cases`. It can also apply customizable parameters to build
different Reference Stack Architecture types.

To run the configuration menu:

.. code-block:: console

  kas menu kronos/Kconfig

.. note::
  To build and run any image for the Kronos FVP the user has to accept its
  `EULA`_, which can be done by selecting the corresponding configuration
  option in the build setup. The Safety Island Actuation Demo is built as
  part of the default deployment.

.. image:: ../images/kronos_reference_stack_build_config.png
   :align: center
   :width: 60 %

|

FVP
---

The ``runfvp`` tool that invokes the Kronos FVP creates one tmux terminal
window per processing element. The default window displayed will be that of the
Primary Compute terminal titled as ``terminal_ns_uart0``. User may press
``Ctrl-b w`` to see the list of tmux windows and use arrow keys to navigate
through the windows and press ``Enter`` to select any processing element
terminal.

The Reference Stack running on the Primary Compute can be logged into as
``root`` user without password in the Linux terminal.

.. note::
  FVPs, and Fast Models in general, are functionally accurate, meaning that they
  fully execute all instructions correctly, however they are not cycle accurate.
  The main goal of the Reference Stack is to prove functionality only, and
  should not be used for performance analysis.

.. _user_guide_reproduce_actuation_demo:

Safety Island Actuation Demo
============================

The demo can be run on the Baremetal Architecture or Virtualization
Architecture. See :ref:`design_applications_actuation` for further details.

Baremetal Architecture
----------------------

Build
^^^^^

To run the configuration menu:

.. code-block:: console

  kas menu kronos/Kconfig

To build a Baremetal Architecture image:

1. Select ``Safety Island Actuation Demo`` from the ``Use-Case`` menu.
2. Choose ``Baremetal`` from the ``Reference Stack Architecture`` menu.
3. Then choose ``Save & Build``.

Run FVP
^^^^^^^

To start the FVP and connect to the Primary Compute terminal (running Linux):

.. code-block:: console

  kas shell -c "../layers/meta-arm/scripts/runfvp -t tmux --verbose"

The user should wait for the system to boot and for the Linux prompt to appear.
Following image shows a example on how the terminal should look like after the
fvp invocation.

  .. image:: ../images/kronos_reference_stack_fvp_run.png
   :align: center

|

The Safety Island (SI) Cluster 2 terminal running the Actuation Service is
available via the tmux window titled ``terminal_uart_si_cluster2``. For ease of
navigation, we recommend joining the SI Cluster 2 terminal window to Primary
Compute terminal window and to create a tmux pane attached to the build host
machine in order to issue commands on it. User can navigate through the panes
by pressing ``Ctrl-b w`` and arrow keys followed by the ``Enter`` key.

Follow the steps below to achieve the same:

1. Press ``Ctrl-b w`` from the tmux session, navigate to the tmux window titled
   ``terminal_ns_uart0`` followed by pressing ``Enter`` key.
2. Press ``Ctrl-b %`` to add a new tmux window which will be used to issue
   commands on the build host machine.
3. Press ``Ctrl-b :`` and then type ``join-pane -s :terminal_uart_si_cluster2``
   followed by pressing ``Enter`` key to join the SI Cluster 2 terminal window
   to Primary Compute terminal window.

Please refer to the following image of the tmux panes re-arrangement.

  .. image:: ../images/kronos_reference_stack_fvp_rearrange_windows.png
    :align: center

|

The Reference Stack running on the Primary Compute can be logged into as
``root`` user without password in the Linux terminal. Run the below
command to guarantee that all the expected services have been
initialized.

   .. code-block:: shell

      systemctl is-system-running --wait

Wait for it to return expecting ``running`` to be printed in the terminal.

Run the demo
^^^^^^^^^^^^

1. Run the ``ping`` command from the Primary Compute terminal (running Linux)
   to verify that it can communicate with the Safety Island (running Zephyr):

   .. code-block:: shell

      # On the Primary Compute terminal
      ping 192.168.2.1 -c 10

   The output should look like the following line, repeated 10 times:

   .. code-block:: shell

      64 bytes from 192.168.2.1 seq=0 ttl=64 time=0.151 ms

2. From the tmux pane started for the build host machine terminal, start the
   Packet Analyzer:

   .. code-block:: shell

      cd ~/kronos/
      # Start the Packet Analyzer
      kas shell -c "oe-run-native packet-analyzer-native start_analyzer -L debug -a localhost -c ./data"

   The following messages are expected from the host terminal:

   .. code-block:: shell

      INFO : analyzer_client.py/_connect_to: Starting analyze, use Ctrl-C to stop the process.
      INFO : analyzer_client.py/_connect_to: Attempting a connect to (localhost : 49152)
      INFO : analyzer_client.py/_connect_to: Successfully connected to (localhost : 49152)

   A message similar to the following should appear on the SI Cluster 2
   terminal:

   .. code-block:: shell

      Actuation Service initialized.
      Accepted tcp connection from the Packet Analyzer: <11>

   Please refer to the following image for an invocation example of the Packet
   Analyzer.

     .. image:: ../images/kronos_reference_stack_packet_analyzer_baremetal.png
       :align: center

|

3. Start the Player on the Primary Compute terminal which replays a recording
   of a driving scenario:

   .. code-block:: shell

      actuation_player -p /usr/share/actuation_player/

   A message similar to the following should appear on the SI Cluster 2
   terminal:

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

4. To shutdown the FVP and terminate the emulation, follow the below steps:

    * Issue a ``shutdown now`` on the Primary Compute terminal. The below
      messages indicate the shutdown process is complete.

      .. code-block:: shell

         [  OK  ] Finished System Power Off.
         [  OK  ] Reached target System Power Off.

    * Close the Primary Compute terminal tmux window created by the ``runfvp``
      tool by pressing ``Ctrl-]`` and typing ``quit``.
    * Close the tmux pane started for the build host machine by pressing
      ``Ctrl-d``.
    * Select the terminal titled as ``python3`` where the ``runfvp`` was
      launched by pressing ``Ctrl-b 0`` and press ``Ctrl-c`` to stop the FVP
      process.

Automated Validation
^^^^^^^^^^^^^^^^^^^^

To run the configuration menu:

.. code-block:: console

  kas menu kronos/Kconfig

To enable the validation tests:
  1. Select ``Safety Island Actuation Demo`` as ``Use-Case``.
  2. Choose ``Baremetal`` from the ``Reference Stack Architecture`` menu.
  3. Choose ``Run Automated Validation`` from the ``Runtime Validation Setup``
     menu.
  4. Then choose ``Save & Build``.

The complete test suit takes around 16 minutes to complete. See
:ref:`validation_actuation_demo` for more details. A similar output to the
following is printed out.

      .. code-block:: console

        NOTE: Executing Tasks
        2023-09-11 20:13:00 - INFO     - Creating terminal default on terminal_ns_uart0
        2023-09-11 20:13:09 - INFO     - Creating terminal tf-a on terminal_sec_uart
        2023-09-11 20:13:09 - INFO     - Creating terminal scp on terminal_uart_scp
        2023-09-11 20:13:09 - INFO     - Creating terminal lcp on terminal_uart_lcp
        2023-09-11 20:13:09 - INFO     - Creating terminal rss on terminal_rss_uart
        2023-09-11 20:13:09 - INFO     - Creating terminal safety_island_c0 on terminal_uart_si_cluster0
        2023-09-11 20:13:10 - INFO     - Creating terminal safety_island_c1 on terminal_uart_si_cluster1
        2023-09-11 20:13:10 - INFO     - Creating terminal safety_island_c2 on terminal_uart_si_cluster2
        2023-09-11 20:13:10 - INFO     - default: Waiting for login prompt
        2023-09-11 20:29:25 - INFO     - RESULTS:
        2023-09-11 20:29:25 - INFO     - RESULTS - test_10_linuxlogin.LinuxLoginTest.test_linux_login: PASSED (17.63s)
        2023-09-11 20:29:25 - INFO     - RESULTS - test_10_safety_island_c1.SafetyIslandC1Test.test_cluster1: PASSED (0.00s)
        2023-09-11 20:29:25 - INFO     - RESULTS - test_10_safety_island_c2.SafetyIslandC2Test.test_cluster2: PASSED (0.00s)
        2023-09-11 20:29:25 - INFO     - RESULTS - test_30_actuation.ActuationTest.test_analyzer_help: PASSED (0.28s)
        2023-09-11 20:29:25 - INFO     - RESULTS - test_30_actuation.ActuationTest.test_ping: PASSED (17.19s)
        2023-09-11 20:29:25 - INFO     - RESULTS - test_30_actuation.ActuationTest.test_player_to_analyzer: PASSED (100.88s)
        2023-09-11 20:29:25 - INFO     - RESULTS - test_40_parsec.ParsecTest.test_parsec_demo: PASSED (374.00s)
        2023-09-11 20:29:25 - INFO     - RESULTS - test_00_lcp.LcpTest.test_normal_boot: PASSED (0.00s)
        2023-09-11 20:29:25 - INFO     - RESULTS - test_00_rss.RssTest.test_normal_boot: PASSED (0.00s)
        2023-09-11 20:29:25 - INFO     - RESULTS - test_00_scp.ScpTest.test_normal_boot: PASSED (0.00s)
        2023-09-11 20:29:25 - INFO     - RESULTS - test_00_secure_partition.OpteeTest.test_optee_normal: PASSED (0.00s)
        2023-09-11 20:29:25 - INFO     - RESULTS - test_00_trusted_firmware_a.TrustedFirmwareTest.test_normal_boot: PASSED (0.00s)
        2023-09-11 20:29:25 - INFO     - RESULTS - test_10_linuxboot.LinuxBootTest.test_linux_boot: PASSED (297.81s)
        2023-09-11 20:29:25 - INFO     - RESULTS - test_20_bsp.BspTest.test_cpu_hotplug: PASSED (115.08s)
        2023-09-11 20:29:25 - INFO     - RESULTS - test_20_bsp.BspTest.test_networking: PASSED (16.50s)
        2023-09-11 20:29:25 - INFO     - RESULTS - test_20_bsp.BspTest.test_rtc: PASSED (9.51s)
        2023-09-11 20:29:25 - INFO     - RESULTS - test_20_bsp.BspTest.test_virtiorng: PASSED (10.21s)
        2023-09-11 20:29:25 - INFO     - RESULTS - test_20_bsp.BspTest.test_watchdog: PASSED (6.50s)
        2023-09-11 20:29:25 - INFO     - RESULTS - test_40_parsec.ParsecTest.test_parsec: SKIPPED (0.00s)
        2023-09-11 20:29:25 - INFO     - SUMMARY:
        2023-09-11 20:29:25 - INFO     - baremetal-image () - Ran 19 tests in 965.595s

The following messages are expected to validate this Use-Case:

      .. code-block:: console
    
        2023-09-11 20:29:25 - INFO     - RESULTS - test_30_actuation.ActuationTest.test_analyzer_help: PASSED (0.28s)
        2023-09-11 20:29:25 - INFO     - RESULTS - test_30_actuation.ActuationTest.test_ping: PASSED (17.19s)
        2023-09-11 20:29:25 - INFO     - RESULTS - test_30_actuation.ActuationTest.test_player_to_analyzer: PASSED (100.88s)

Virtualization Architecture
---------------------------

Build
^^^^^

To run the configuration menu:

.. code-block:: console

  kas menu kronos/Kconfig

To build a Virtualization Architecture image:

1. Select ``Safety Island Actuation Demo`` from the ``Use-Case`` menu.
2. Choose ``Virtualization`` from the ``Reference Stack Architecture`` menu.
3. Then choose ``Save & Build``.

Run FVP
^^^^^^^

To start the FVP and connect to the Primary Compute terminal (running Linux):

.. code-block:: console

  kas shell -c "../layers/meta-arm/scripts/runfvp -t tmux --verbose"

The user should wait for the system to boot and for the Linux prompt to appear.
On a Virtualization Architecture image, this will access Dom0 terminal.
Following image shows a example on how the terminal should look like after the
fvp invocation.

  .. image:: ../images/kronos_reference_stack_fvp_run.png
   :align: center

|

The Safety Island (SI) Cluster 2 terminal running the Actuation Service is
available via the tmux window titled ``terminal_uart_si_cluster2``. For ease of
navigation, we recommend joining the SI Cluster 2 terminal window to Primary
Compute terminal window and to create a tmux pane attached to the build host
machine in order to issue commands on it. User can navigate through the panes
by pressing ``Ctrl-b w`` and arrow keys followed by the ``Enter`` key.

Follow the steps below to achieve the same:

1. Press ``Ctrl-b w`` from the tmux session, navigate to the tmux window titled
   ``terminal_ns_uart0`` followed by pressing ``Enter`` key.
2. Press ``Ctrl-b %`` to add a new tmux window which will be used to issue
   commands on the build host machine.
3. Press ``Ctrl-b :`` and then type ``join-pane -s :terminal_uart_si_cluster2``
   followed by pressing ``Enter`` key to join the SI Cluster 2 terminal window
   to Primary Compute terminal window.

Please refer to the following image of the tmux panes re-arrangement.

  .. image:: ../images/kronos_reference_stack_fvp_rearrange_windows.png
    :align: center

|

The Reference Stack running on the Primary Compute can be logged into as
``root`` user without password in the Linux terminal. Run the below
command to guarantee that all the expected services have been
initialized.

   .. code-block:: shell

      systemctl is-system-running --wait

Wait for it to return expecting ``running`` to be printed in the terminal.

Run the Demo
^^^^^^^^^^^^

1. Enter the DomU1 console using the ``xl`` tool:

   .. code-block:: shell

      xl console domu1

DomU1 can be logged into as ``root`` user without password in the Linux
terminal. This command will provide a console on the DomU1. To exit, one can
enter ``Ctrl-]`` (to access the FVP telnet shell), followed by typing
``send esc`` into the telnet shell and pressing ``Enter``. See the
`xl documentation`_ for further details.

2. Run the ``ping`` command from the Primary Compute terminal (running Linux)
   to verify that it can communicate with the Safety Island (running Zephyr):

   .. code-block:: shell

      # On the Primary Compute terminal
      ping 192.168.2.1 -c 10

   The output should look like the following line, repeated 10 times:

   .. code-block:: shell

      64 bytes from 192.168.2.1 seq=0 ttl=64 time=0.151 ms

3. From the tmux pane started for the build host machine terminal, start the
   Packet Analyzer:

   .. code-block:: shell

      cd ~/kronos/
      # Start the Packet Analyzer
      kas shell -c "oe-run-native packet-analyzer-native start_analyzer -L debug -a localhost -c ./data"

   The following messages are expected from the host terminal:

   .. code-block:: shell

      INFO : analyzer_client.py/_connect_to: Starting analyze, use Ctrl-C to stop the process.
      INFO : analyzer_client.py/_connect_to: Attempting a connect to (localhost : 49152)
      INFO : analyzer_client.py/_connect_to: Successfully connected to (localhost : 49152)

   A message similar to the following should appear on the SI Cluster 2
   terminal:

   .. code-block:: shell

      Actuation Service initialized.
      Accepted tcp connection from the Packet Analyzer: <11>

   Please refer to the following image for an invocation example of the Packet
   Analyzer.

     .. image:: ../images/kronos_reference_stack_packet_analyzer_virtualization.png
       :align: center

|

4. Start the Player on the Primary Compute which replays a recording of a
   driving scenario:

   .. code-block:: shell

      actuation_player -p /usr/share/actuation_player/

   A message similar to the following should appear on the SI Cluster 2
   terminal:

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

5. To leave the DomU1 console, type ``Ctrl-]`` and enter ``send esc``.

6. To shutdown the FVP and terminate the emulation, follow the below steps:

    * Issue a ``shutdown now`` on the Primary Compute terminal. The below
      messages indicate the shutdown process is complete.

      .. code-block:: shell

         [  OK  ] Finished System Power Off.
         [  OK  ] Reached target System Power Off.

    * Close the Primary Compute terminal tmux window created by the ``runfvp``
      tool by pressing ``Ctrl-]`` and typing ``quit``.
    * Close the tmux pane started for the build host machine by pressing
      ``Ctrl-d``.
    * Select the terminal titled as ``python3`` where the ``runfvp`` was
      launched by pressing ``Ctrl-b 0`` and press ``Ctrl-c`` to stop the FVP
      process.

Automated Validation
^^^^^^^^^^^^^^^^^^^^

To run the configuration menu:

.. code-block:: console

  kas menu kronos/Kconfig

To enable the validation tests:
  1. Select ``Safety Island Actuation Demo`` as ``Use-Case``.
  2. Choose ``Virtualization`` from the ``Reference Stack Architecture`` menu.
  3. Choose ``Run Automated Validation`` from the ``Runtime Validation Setup``
     menu.
  4. Then choose ``Save & Build``.

The complete test suit takes around 41 minutes to complete. See
:ref:`validation_actuation_demo` for more details. A similar output to the
following is printed out.

  .. code-block:: console

    2023-09-11 20:20:46 - INFO     - Creating terminal default on terminal_ns_uart0
    2023-09-11 20:20:56 - INFO     - Creating terminal tf-a on terminal_sec_uart
    2023-09-11 20:20:56 - INFO     - Creating terminal scp on terminal_uart_scp
    2023-09-11 20:20:56 - INFO     - Creating terminal lcp on terminal_uart_lcp
    2023-09-11 20:20:56 - INFO     - Creating terminal rss on terminal_rss_uart
    2023-09-11 20:20:56 - INFO     - Creating terminal safety_island_c0 on terminal_uart_si_cluster0
    2023-09-11 20:20:56 - INFO     - Creating terminal safety_island_c1 on terminal_uart_si_cluster1
    2023-09-11 20:20:57 - INFO     - Creating terminal safety_island_c2 on terminal_uart_si_cluster2
    2023-09-11 20:20:57 - INFO     - default: Waiting for login prompt
    2023-09-11 20:42:20 - INFO     - Test skipped due to reliance on FFA, not supported in virtualization
    2023-09-11 20:42:36 - INFO     - 'rtc' not tested in DomU
    2023-09-11 20:42:36 - INFO     - 'virtiorng' not tested in DomU
    2023-09-11 20:42:36 - INFO     - 'watchdog' not tested in DomU
    2023-09-11 20:42:53 - INFO     - 'rtc' not tested in DomU
    2023-09-11 20:42:53 - INFO     - 'virtiorng' not tested in DomU
    2023-09-11 20:42:53 - INFO     - 'watchdog' not tested in DomU
    2023-09-11 20:44:53 - INFO     - Test skipped due to reliance on FFA, not supported in virtualization
    2023-09-11 20:46:38 - INFO     - Test skipped due to reliance on FFA, not supported in virtualization
    2023-09-11 21:02:21 - INFO     - RESULTS:
    2023-09-11 21:02:21 - INFO     - RESULTS - test_10_linuxlogin.LinuxLoginTest.test_linux_login: PASSED (617.49s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_10_safety_island_c1.SafetyIslandC1Test.test_cluster1: PASSED (0.00s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_10_safety_island_c2.SafetyIslandC2Test.test_cluster2: PASSED (0.00s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_30_actuation.ActuationTest.test_analyzer_help: PASSED (0.28s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_30_actuation.ActuationTest.test_ping: PASSED (30.62s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_30_actuation.ActuationTest.test_player_to_analyzer: PASSED (170.71s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_40_parsec.ParsecTest.test_parsec: PASSED (120.99s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_40_virtualization.BspTestDomU1.test_cpu_hotplug: PASSED (7.98s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_40_virtualization.BspTestDomU1.test_networking: PASSED (2.65s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_40_virtualization.BspTestDomU2.test_cpu_hotplug: PASSED (2.09s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_40_virtualization.BspTestDomU2.test_networking: PASSED (2.19s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_40_virtualization.GICv4DomU1Test.test_gicv4_1: PASSED (0.58s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_40_virtualization.ParsecDomU1Test.test_parsec: PASSED (92.72s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_40_virtualization.ParsecDomU2Test.test_parsec: PASSED (91.38s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_40_virtualization.PtestRunnerDom0Test.test_ptestrunner: PASSED (850.17s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_00_lcp.LcpTest.test_normal_boot: PASSED (0.00s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_00_rss.RssTest.test_normal_boot: PASSED (0.00s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_00_scp.ScpTest.test_normal_boot: PASSED (0.00s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_00_trusted_firmware_a.TrustedFirmwareTest.test_normal_boot: PASSED (0.00s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_10_linuxboot.LinuxBootTest.test_linux_boot: PASSED (317.42s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_20_bsp.BspTest.test_cpu_hotplug: PASSED (20.22s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_20_bsp.BspTest.test_networking: PASSED (21.98s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_20_bsp.BspTest.test_rtc: PASSED (9.90s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_20_bsp.BspTest.test_virtiorng: PASSED (10.04s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_20_bsp.BspTest.test_watchdog: PASSED (6.59s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_40_parsec.ParsecTest.test_parsec_demo: SKIPPED (0.00s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_40_virtualization.BspTestDomU1.test_rtc: SKIPPED (0.00s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_40_virtualization.BspTestDomU1.test_virtiorng: SKIPPED (0.00s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_40_virtualization.BspTestDomU1.test_watchdog: SKIPPED (0.00s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_40_virtualization.BspTestDomU2.test_rtc: SKIPPED (0.00s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_40_virtualization.BspTestDomU2.test_virtiorng: SKIPPED (0.00s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_40_virtualization.BspTestDomU2.test_watchdog: SKIPPED (0.00s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_40_virtualization.ParsecDomU1Test.test_parsec_demo: SKIPPED (0.00s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_40_virtualization.ParsecDomU2Test.test_parsec_demo: SKIPPED (0.00s)
    2023-09-11 21:02:21 - INFO     - SUMMARY:
    2023-09-11 21:02:21 - INFO     - virtualization-image () - Ran 34 tests in 2469.165s

The following messages are expected to validate this Use-Case:

  .. code-block:: console

    2023-09-11 21:02:21 - INFO     - RESULTS - test_30_actuation.ActuationTest.test_analyzer_help: PASSED (0.28s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_30_actuation.ActuationTest.test_ping: PASSED (30.62s)
    2023-09-11 21:02:21 - INFO     - RESULTS - test_30_actuation.ActuationTest.test_player_to_analyzer: PASSED (170.71s)

Safety Island Communication Demo (using HIPC)
=============================================

The Safety Island Communication Demo uses :ref:`HIPC (Heterogeneous
Inter-processor Communication) <design/hipc:Heterogeneous Inter-processor
Communication (HIPC)>` to validate networking between the Primary Compute and
the three Safety Island clusters. ``ping`` and ``iperf`` tools are installed.

Baremetal Architecture
----------------------

Build
^^^^^

To run the configuration menu:

.. code-block:: console

  kas menu kronos/Kconfig

To build a Baremetal Architecture image:

1. Select ``Safety Island Communication Demo (using HIPC)`` from the
   ``Use-Case`` menu.
2. Choose ``Baremetal`` from the ``Reference Stack Architecture`` menu.
3. Then choose ``Save & Build``.

Automated Validation
^^^^^^^^^^^^^^^^^^^^

To run the configuration menu:

.. code-block:: console

  kas menu kronos/Kconfig

To enable the validation tests:

  1. Select ``Safety Island Communication Demo (using HIPC)`` as ``Use-Case``.
  2. Choose ``Baremetal`` from the ``Reference Stack Architecture`` menu.
  3. Choose ``Run Automated Validation`` from the ``Runtime Validation Setup``
     menu.
  4. Then choose ``Save & Build``.

A similar output to the following is printed out. The complete test suit takes
around 17 minutes to complete. See
:ref:`validation_hipc_demo` for more details.

.. code-block:: console

  2023-11-05 21:17:10 - INFO     - Creating terminal default on terminal_ns_uart0
  2023-11-05 21:17:20 - INFO     - Creating terminal tf-a on terminal_sec_uart
  2023-11-05 21:17:20 - INFO     - Creating terminal scp on terminal_uart_scp
  2023-11-05 21:17:20 - INFO     - Creating terminal lcp on terminal_uart_lcp
  2023-11-05 21:17:20 - INFO     - Creating terminal rss on terminal_rss_uart
  2023-11-05 21:17:21 - INFO     - Creating terminal safety_island_c0 on terminal_uart_si_cluster0
  2023-11-05 21:17:21 - INFO     - Creating terminal safety_island_c1 on terminal_uart_si_cluster1
  2023-11-05 21:17:22 - INFO     - Creating terminal safety_island_c2 on terminal_uart_si_cluster2
  2023-11-05 21:17:22 - INFO     - default: Waiting for login prompt
  2023-11-05 21:34:42 - INFO     - RESULTS:
  2023-11-05 21:34:42 - INFO     - RESULTS - test_10_linuxlogin.LinuxLoginTest.test_linux_login: PASSED (20.02s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_hipc_cluster0: PASSED (116.47s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_hipc_cluster1: PASSED (129.01s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_hipc_cluster2: PASSED (146.54s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_hipc_cluster_cl0_cl1: PASSED (27.87s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_hipc_cluster_cl0_cl2: PASSED (32.90s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_hipc_cluster_cl1_cl2: PASSED (54.07s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_ping_cl0_cl1: PASSED (18.51s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_ping_cl0_cl2: PASSED (18.73s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_ping_cl1_cl2: PASSED (18.92s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_ping_cluster0: PASSED (50.29s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_ping_cluster1: PASSED (47.85s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_ping_cluster2: PASSED (46.58s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_ptp.PTPTest.test_ptp_linux_services: PASSED (1.87s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_ptp.PTPTest.test_ptp_si_clients: PASSED (16.93s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_00_lcp.LcpTest.test_normal_boot: PASSED (0.00s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_00_rss.RssTest.test_normal_boot: PASSED (0.00s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_00_scp.ScpTest.test_normal_boot: PASSED (0.00s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_00_secure_partition.OpteeTest.test_optee_normal: PASSED (0.00s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_00_trusted_firmware_a.TrustedFirmwareTest.test_normal_boot: PASSED (0.00s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_10_linuxboot.LinuxBootTest.test_linux_boot: PASSED (283.33s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_ptp.PTPTestDomU1.test_ptp_domu_client: SKIPPED
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_ptp.PTPTestDomU1.test_ptp_linux_services: SKIPPED
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_ptp.PTPTestDomU2.test_ptp_domu_client: SKIPPED
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_ptp.PTPTestDomU2.test_ptp_linux_services: SKIPPED
  2023-11-05 21:34:42 - INFO     - SUMMARY:
  2023-11-05 21:34:42 - INFO     - baremetal-image () - Ran 21 tests in 1030.628s

The following messages are expected to validate this Use-Case:

.. code-block:: console

  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_hipc_cluster0: PASSED (116.47s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_hipc_cluster1: PASSED (129.01s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_hipc_cluster2: PASSED (146.54s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_hipc_cluster_cl0_cl1: PASSED (27.87s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_hipc_cluster_cl0_cl2: PASSED (32.90s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_hipc_cluster_cl1_cl2: PASSED (54.07s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_ping_cl0_cl1: PASSED (18.51s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_ping_cl0_cl2: PASSED (18.73s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_ping_cl1_cl2: PASSED (18.92s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_ping_cluster0: PASSED (50.29s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_ping_cluster1: PASSED (47.85s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_hipc.HIPCTestBase.test_ping_cluster2: PASSED (46.58s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_ptp.PTPTest.test_ptp_linux_services: PASSED (1.87s)
  2023-11-05 21:34:42 - INFO     - RESULTS - test_30_ptp.PTPTest.test_ptp_si_clients: PASSED (16.93s)

Virtualization Architecture
---------------------------

Build
^^^^^

To run the configuration menu:

.. code-block:: console

  kas menu kronos/Kconfig

To build a Virtualization Architecture image:

1. Select ``Safety Island Communication Demo (using HIPC)`` as ``Use-Case``.
2. Choose ``Virtualization`` from the ``Reference Stack Architecture`` menu.
3. Then choose ``Save & Build``.


Automated Validation
^^^^^^^^^^^^^^^^^^^^

To run the configuration menu:

.. code-block:: console

  kas menu kronos/Kconfig

To enable the validation tests:
  1. Select ``Safety Island Communication Demo (using HIPC)`` as ``Use-Case``.
  2. Choose ``Virtualization`` from the ``Reference Stack Architecture`` menu.
  3. Choose ``Run Automated Validation`` from the ``Runtime Validation Setup``
     menu.
  4. Then choose ``Save & Build``.

The complete test suit takes around 41 minutes to complete. See
:ref:`validation_hipc_demo` for more details. A similar output to the
following is printed out.

.. code-block:: console

  2023-11-05 21:17:43 - INFO     - Creating terminal default on terminal_ns_uart0
  2023-11-05 21:17:52 - INFO     - Creating terminal tf-a on terminal_sec_uart
  2023-11-05 21:17:52 - INFO     - Creating terminal scp on terminal_uart_scp
  2023-11-05 21:17:53 - INFO     - Creating terminal lcp on terminal_uart_lcp
  2023-11-05 21:17:53 - INFO     - Creating terminal rss on terminal_rss_uart
  2023-11-05 21:17:53 - INFO     - Creating terminal safety_island_c0 on terminal_uart_si_cluster0
  2023-11-05 21:17:53 - INFO     - Creating terminal safety_island_c1 on terminal_uart_si_cluster1
  2023-11-05 21:17:53 - INFO     - Creating terminal safety_island_c2 on terminal_uart_si_cluster2
  2023-11-05 21:17:53 - INFO     - default: Waiting for login prompt
  2023-11-05 21:49:05 - INFO     - HIPC to Cluster 0 not tested for DomU2
  2023-11-05 21:52:43 - INFO     - HIPC to Cluster 2 not tested for DomU2
  2023-11-05 21:56:46 - INFO     - Ping to Cluster 0 not tested for DomU2
  2023-11-05 21:56:46 - INFO     - Ping to Cluster 2 not tested for DomU2
  2023-11-05 21:59:07 - INFO     - RESULTS:
  2023-11-05 21:59:07 - INFO     - RESULTS - test_10_linuxlogin.LinuxLoginTest.test_linux_login: PASSED (576.25s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_hipc_cluster0: PASSED (177.82s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_hipc_cluster1: PASSED (133.61s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_hipc_cluster2: PASSED (154.19s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_hipc_cluster_cl0_cl1: PASSED (37.69s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_hipc_cluster_cl0_cl2: PASSED (34.65s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_hipc_cluster_cl1_cl2: PASSED (57.19s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_ping_cl0_cl1: PASSED (35.90s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_ping_cl0_cl2: PASSED (35.35s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_ping_cl1_cl2: PASSED (35.49s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_ping_cluster0: PASSED (91.98s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_ping_cluster1: PASSED (89.69s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_ping_cluster2: PASSED (89.12s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU2.test_hipc_cluster1: PASSED (127.11s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU2.test_hipc_cluster_cl0_cl1: PASSED (37.59s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU2.test_hipc_cluster_cl0_cl2: PASSED (41.76s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU2.test_hipc_cluster_cl1_cl2: PASSED (57.34s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU2.test_ping_cl0_cl1: PASSED (35.54s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU2.test_ping_cl0_cl2: PASSED (35.36s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU2.test_ping_cl1_cl2: PASSED (35.85s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU2.test_ping_cluster1: PASSED (90.17s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_ptp.PTPTest.test_ptp_linux_services: PASSED (3.57s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_ptp.PTPTest.test_ptp_si_clients: PASSED (25.89s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_ptp.PTPTestDomU1.test_ptp_domu_client: PASSED (28.75s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_ptp.PTPTestDomU1.test_ptp_linux_services: PASSED (0.67s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_ptp.PTPTestDomU2.test_ptp_domu_client: PASSED (25.30s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_ptp.PTPTestDomU2.test_ptp_linux_services: PASSED (0.69s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_00_lcp.LcpTest.test_normal_boot: PASSED (0.00s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_00_rss.RssTest.test_normal_boot: PASSED (0.00s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_00_scp.ScpTest.test_normal_boot: PASSED (0.00s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_00_trusted_firmware_a.TrustedFirmwareTest.test_normal_boot: PASSED (0.00s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_10_linuxboot.LinuxBootTest.test_linux_boot: PASSED (302.13s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU2.test_hipc_cluster0: SKIPPED (0.00s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU2.test_hipc_cluster2: SKIPPED (0.00s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU2.test_ping_cluster0: SKIPPED (0.00s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU2.test_ping_cluster2: SKIPPED (0.00s)
  2023-11-05 21:59:07 - INFO     - SUMMARY:
  2023-11-05 21:59:07 - INFO     - virtualization-image () - Ran 36 tests in 2461.116s

The following messages are expected to validate this Use-Case:

.. code-block:: console

  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_hipc_cluster0: PASSED (177.82s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_hipc_cluster1: PASSED (133.61s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_hipc_cluster2: PASSED (154.19s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_hipc_cluster_cl0_cl1: PASSED (37.69s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_hipc_cluster_cl0_cl2: PASSED (34.65s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_hipc_cluster_cl1_cl2: PASSED (57.19s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_ping_cl0_cl1: PASSED (35.90s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_ping_cl0_cl2: PASSED (35.35s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_ping_cl1_cl2: PASSED (35.49s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_ping_cluster0: PASSED (91.98s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_ping_cluster1: PASSED (89.69s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_ping_cluster2: PASSED (89.12s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU2.test_hipc_cluster1: PASSED (127.11s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU2.test_hipc_cluster_cl0_cl1: PASSED (37.59s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU2.test_hipc_cluster_cl0_cl2: PASSED (41.76s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU2.test_hipc_cluster_cl1_cl2: PASSED (57.34s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU2.test_ping_cl0_cl1: PASSED (35.54s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU2.test_ping_cl0_cl2: PASSED (35.36s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU2.test_ping_cl1_cl2: PASSED (35.85s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_hipc_virtualization.HIPCTestDomU2.test_ping_cluster1: PASSED (90.17s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_ptp.PTPTest.test_ptp_linux_services: PASSED (3.57s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_ptp.PTPTest.test_ptp_si_clients: PASSED (25.89s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_ptp.PTPTestDomU1.test_ptp_domu_client: PASSED (28.75s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_ptp.PTPTestDomU1.test_ptp_linux_services: PASSED (0.67s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_ptp.PTPTestDomU2.test_ptp_domu_client: PASSED (25.30s)
  2023-11-05 21:59:07 - INFO     - RESULTS - test_30_ptp.PTPTestDomU2.test_ptp_linux_services: PASSED (0.69s)

Parsec-enabled TLS Demo
=======================

The demo is always available when the ``Baremetal Architecture`` is selected.
The demo consists of a TLS server and a TLS client. Please refer to
:ref:`design_applications_parsec_enabled_tls` for more information on
this application. This demo is included as part of the
``Safety Island Actuation Demo``.

Baremetal Architecture
----------------------

Build
^^^^^

To run the configuration menu:

.. code-block:: console

  kas menu kronos/Kconfig

To build a Baremetal Architecture image:

1. Select ``Safety Island Actuation Demo`` from the ``Use-Case`` menu.
2. Choose ``Baremetal`` from the ``Reference Stack Architecture`` menu.
3. Then choose ``Save & Build``.

Run FVP
^^^^^^^

To start the FVP and connect to the Primary Compute terminal (running Linux):

.. code-block:: console

  kas shell -c "../layers/meta-arm/scripts/runfvp -t tmux --verbose"

The user should wait for the system to boot and for the Linux prompt to appear.

The Reference Stack running on the Primary Compute can be logged into as
``root`` user without password in the Linux terminal. Run the below
command to guarantee that all the expected services have been
initialized.

   .. code-block:: shell

      systemctl is-system-running --wait

Wait for it to return expecting ``running`` to be printed in the terminal.

Run the demo
^^^^^^^^^^^^

The demo consists of a TLS server and a TLS client. Please refer to
:ref:`design_applications_parsec_enabled_tls` for more information on
this application.

1. Run ``ssl_server`` from the Primary Compute terminal in the background and
   press ``Enter`` key to continue:

   .. code-block:: shell

      ssl_server &

   A message similar to the following should appear:
   
   .. code-block:: shell

        . Seeding the random number generator... ok
        . Loading the server cert. and key... ok
        . Bind on https://localhost:4433/ ... ok
        . Setting up the SSL data.... ok
        . Waiting for a remote connection ...

   The TLS client can take an optional parameter as the TLS server IP address. The
   default value of the parameter is ``localhost``.

2. Run ``ssl_client1`` from the Primary Compute terminal in a container:

   .. code-block:: shell

      docker run  --rm -v /run/parsec/parsec.sock:/run/parsec/parsec.sock -v /usr/bin/ssl_client1:/usr/bin/ssl_client1 --network host docker.io/library/ubuntu:22.04 ssl_client1

   A message similar to the following should appear:

   .. code-block:: shell

         . Seeding the random number generator... ok
         . Loading the CA root certificate ... ok (0 skipped)
         . Connecting to tcp/localhost/4433... ok
         . Setting up the SSL/TLS structure... ok
         . Performing the SSL/TLS handshake... ok
         . Verifying peer X.509 certificate... ok
         > Write to server: 18 bytes written

       GET / HTTP/1.0

       < Read from server: 156 bytes read

       HTTP/1.0 200 OK
       Content-Type: text/html

       <h2>mbed TLS Test Server</h2>
       <p>Successful connection using: TLS-ECDHE-RSA-WITH-CHACHA20-POLY1305-SHA256</p>

3. Stop the TLS server and synchronize the container image to the
   persistent storage:

     .. code-block:: shell
  
        pkill ssl_server
        sync

4. To shutdown the FVP and terminate the emulation, follow the below step:

    * Issue a ``shutdown now`` on the Primary Compute terminal. The below
      messages indicate the shutdown process is complete.

      .. code-block:: shell

         [  OK  ] Finished System Power Off.
         [  OK  ] Reached target System Power Off.

    * Select the terminal titled as ``python3`` where the ``runfvp`` was
      launched by pressing ``Ctrl-b 0`` and press ``Ctrl-c`` to stop the FVP
      process.

Automated Validation
^^^^^^^^^^^^^^^^^^^^

For more details about the validation of Parsec demo, refer to
:ref:`validation_parsec_enabled_tls_demo`.

To run the configuration menu:

.. code-block:: console

  kas menu kronos/Kconfig

To enable the validation tests:
  1. Select ``Safety Island Actuation Demo`` as ``Use-Case``.
  2. Choose ``Baremetal Architecture`` from the ``Reference Stack Architecture``
     menu.
  3. Choose ``Run Automated Validation`` from the ``Runtime Validation Setup``
     menu.
  4. Then choose ``Save & Build``.

The complete test suit takes around 16 minutes to complete. See
:ref:`validation_parsec_enabled_tls_demo` for more details. A similar output to
the following is printed out.

.. code-block:: console

  NOTE: Executing Tasks
  2023-09-11 20:13:00 - INFO     - Creating terminal default on terminal_ns_uart0
  2023-09-11 20:13:09 - INFO     - Creating terminal tf-a on terminal_sec_uart
  2023-09-11 20:13:09 - INFO     - Creating terminal scp on terminal_uart_scp
  2023-09-11 20:13:09 - INFO     - Creating terminal lcp on terminal_uart_lcp
  2023-09-11 20:13:09 - INFO     - Creating terminal rss on terminal_rss_uart
  2023-09-11 20:13:09 - INFO     - Creating terminal safety_island_c0 on terminal_uart_si_cluster0
  2023-09-11 20:13:10 - INFO     - Creating terminal safety_island_c1 on terminal_uart_si_cluster1
  2023-09-11 20:13:10 - INFO     - Creating terminal safety_island_c2 on terminal_uart_si_cluster2
  2023-09-11 20:13:10 - INFO     - default: Waiting for login prompt
  2023-09-11 20:29:25 - INFO     - RESULTS:
  2023-09-11 20:29:25 - INFO     - RESULTS - test_10_linuxlogin.LinuxLoginTest.test_linux_login: PASSED (17.63s)
  2023-09-11 20:29:25 - INFO     - RESULTS - test_10_safety_island_c1.SafetyIslandC1Test.test_cluster1: PASSED (0.00s)
  2023-09-11 20:29:25 - INFO     - RESULTS - test_10_safety_island_c2.SafetyIslandC2Test.test_cluster2: PASSED (0.00s)
  2023-09-11 20:29:25 - INFO     - RESULTS - test_30_actuation.ActuationTest.test_analyzer_help: PASSED (0.28s)
  2023-09-11 20:29:25 - INFO     - RESULTS - test_30_actuation.ActuationTest.test_ping: PASSED (17.19s)
  2023-09-11 20:29:25 - INFO     - RESULTS - test_30_actuation.ActuationTest.test_player_to_analyzer: PASSED (100.88s)
  2023-09-11 20:29:25 - INFO     - RESULTS - test_40_parsec.ParsecTest.test_parsec_demo: PASSED (374.00s)
  2023-09-11 20:29:25 - INFO     - RESULTS - test_00_lcp.LcpTest.test_normal_boot: PASSED (0.00s)
  2023-09-11 20:29:25 - INFO     - RESULTS - test_00_rss.RssTest.test_normal_boot: PASSED (0.00s)
  2023-09-11 20:29:25 - INFO     - RESULTS - test_00_scp.ScpTest.test_normal_boot: PASSED (0.00s)
  2023-09-11 20:29:25 - INFO     - RESULTS - test_00_secure_partition.OpteeTest.test_optee_normal: PASSED (0.00s)
  2023-09-11 20:29:25 - INFO     - RESULTS - test_00_trusted_firmware_a.TrustedFirmwareTest.test_normal_boot: PASSED (0.00s)
  2023-09-11 20:29:25 - INFO     - RESULTS - test_10_linuxboot.LinuxBootTest.test_linux_boot: PASSED (297.81s)
  2023-09-11 20:29:25 - INFO     - RESULTS - test_20_bsp.BspTest.test_cpu_hotplug: PASSED (115.08s)
  2023-09-11 20:29:25 - INFO     - RESULTS - test_20_bsp.BspTest.test_networking: PASSED (16.50s)
  2023-09-11 20:29:25 - INFO     - RESULTS - test_20_bsp.BspTest.test_rtc: PASSED (9.51s)
  2023-09-11 20:29:25 - INFO     - RESULTS - test_20_bsp.BspTest.test_virtiorng: PASSED (10.21s)
  2023-09-11 20:29:25 - INFO     - RESULTS - test_20_bsp.BspTest.test_watchdog: PASSED (6.50s)
  2023-09-11 20:29:25 - INFO     - RESULTS - test_40_parsec.ParsecTest.test_parsec: SKIPPED (0.00s)
  2023-09-11 20:29:25 - INFO     - SUMMARY:
  2023-09-11 20:29:25 - INFO     - baremetal-image () - Ran 19 tests in 965.595s

The following messages are expected to validate this Use-Case:

.. code-block:: console

  2023-09-11 20:29:25 - INFO     - RESULTS - test_40_parsec.ParsecTest.test_parsec_demo: PASSED (374.00s)


Safety Island PSA Secure Storage APIs Architecture Test Suite
=============================================================

The demo is always available when the ``Baremetal Architecture`` is selected.
See :ref:`design_applications_psa_arch_tests` for further details.

Baremetal Architecture
----------------------

Build
^^^^^

To run the configuration menu:

.. code-block:: console

  kas menu kronos/Kconfig

To build a Baremetal Architecture image:

1. Select ``Safety Island PSA Secure Storage APIs Architecture Test Suite``
   from the ``Use-Case`` menu.
2. Then choose ``Save & Build``.

Run FVP
^^^^^^^

To start the FVP:

.. code-block:: console

  kas shell -c "../layers/meta-arm/scripts/runfvp -t tmux --verbose"

The Safety Island (SI) Cluster 2 terminal running the ``PSA Secure Storage APIs
Architecture Test Suite`` is available via the tmux window titled
``terminal_uart_si_cluster2``. User can navigate through the panes by pressing
``Ctrl-b w`` and arrow keys followed by the ``Enter`` key.

Run the tests
^^^^^^^^^^^^^

The tests will automatically run. A log similar to the following should be
visible; it is normal for some tests to be skipped but there should be no
failed tests::

    ***** PSA Architecture Test Suite - Version 1.4 *****
    Running.. Storage Suite
    ******************************************
    TEST: 401 | DESCRIPTION: UID not found check | UT: STORAGE
    [Info] Executing tests from non-secure
    [Info] Executing ITS Tests
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

    [Info] Executing PS Tests
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

Automated Validation
^^^^^^^^^^^^^^^^^^^^

To run the configuration menu:

.. code-block:: console

  kas menu kronos/Kconfig

To enable the validation tests:
  1. Select ``Safety Island PSA Secure Storage APIs Architecture Test Suite``
     from the ``Use-Case`` menu.
  2. Choose ``Run Automated Validation`` from the ``Runtime Validation Setup``
     menu.
  3. Then choose ``Save & Build``.

The complete test suit takes around 10 minutes to complete. See
:ref:`validation_psa_arch_tests` for more details. A similar output to the
following is printed out.

      .. code-block:: console

        NOTE: Executing Tasks
        2023-11-13 11:43:15 - INFO     - Creating terminal default on terminal_ns_uart0
        2023-11-13 11:43:23 - INFO     - Creating terminal tf-a on terminal_sec_uart
        2023-11-13 11:43:23 - INFO     - Creating terminal scp on terminal_uart_scp
        2023-11-13 11:43:23 - INFO     - Creating terminal lcp on terminal_uart_lcp
        2023-11-13 11:43:23 - INFO     - Creating terminal rss on terminal_rss_uart
        2023-11-13 11:43:24 - INFO     - Creating terminal safety_island_c0 on terminal_uart_si_cluster0
        2023-11-13 11:43:24 - INFO     - Creating terminal safety_island_c1 on terminal_uart_si_cluster1
        2023-11-13 11:43:24 - INFO     - Creating terminal safety_island_c2 on terminal_uart_si_cluster2
        2023-11-13 11:43:24 - INFO     - default: Waiting for login prompt
        2023-11-13 11:53:36 - INFO     - Skip as ZEPHYR_APP_SAFETY_ISLAND_CL0 is not psa-storage-tests
        2023-11-13 11:53:36 - INFO     - Skip as ZEPHYR_APP_SAFETY_ISLAND_CL1 is not psa-storage-tests
        2023-11-13 11:53:48 - INFO     - RESULTS:
        2023-11-13 11:53:48 - INFO     - RESULTS - test_10_linuxlogin.LinuxLoginTest.test_linux_login: PASSED (38.58s)
        2023-11-13 11:53:48 - INFO     - RESULTS - test_10_si_psa_arch_tests.SIPSAArchTests.test_psa_si_cluster2: PASSED (0.00s)
        2023-11-13 11:53:48 - INFO     - RESULTS - test_00_lcp.LcpTest.test_normal_boot: PASSED (0.00s)
        2023-11-13 11:53:48 - INFO     - RESULTS - test_00_rss.RssTest.test_gic_multiple_view: PASSED (0.00s)
        2023-11-13 11:53:48 - INFO     - RESULTS - test_00_rss.RssTest.test_ni710ae: PASSED (0.00s)
        2023-11-13 11:53:48 - INFO     - RESULTS - test_00_rss.RssTest.test_normal_boot: PASSED (0.00s)
        2023-11-13 11:53:48 - INFO     - RESULTS - test_00_scp.ScpTest.test_normal_boot: PASSED (0.00s)
        2023-11-13 11:53:48 - INFO     - RESULTS - test_00_secure_partition.OpteeTest.test_optee_normal: PASSED (0.00s)
        2023-11-13 11:53:48 - INFO     - RESULTS - test_00_trusted_firmware_a.TrustedFirmwareTest.test_normal_boot: PASSED (0.00s)
        2023-11-13 11:53:48 - INFO     - RESULTS - test_10_linuxboot.LinuxBootTest.test_linux_boot: PASSED (573.76s)
        2023-11-13 11:53:48 - INFO     - RESULTS - test_10_si_psa_arch_tests.SIPSAArchTests.test_psa_si_cluster0: SKIPPED (0.00s)
        2023-11-13 11:53:48 - INFO     - RESULTS - test_10_si_psa_arch_tests.SIPSAArchTests.test_psa_si_cluster1: SKIPPED (0.00s)
        2023-11-13 11:53:48 - INFO     - SUMMARY:
        2023-11-13 11:53:48 - INFO     - baremetal-image () - Ran 12 tests in 612.344s

The following message is expected to validate this Use-Case:

      .. code-block:: console

        2023-11-13 11:53:48 - INFO     - RESULTS - test_10_si_psa_arch_tests.SIPSAArchTests.test_psa_si_cluster2: PASSED (0.00s)

Safety Island PSA Crypto APIs Architecture Test Suite
=====================================================

The demo is always available when the ``Baremetal Architecture`` is selected.
See :ref:`design_applications_psa_arch_tests` for further details.

Baremetal Architecture
----------------------

Build
^^^^^

To run the configuration menu:

.. code-block:: console

  kas menu kronos/Kconfig

To build a Baremetal Architecture image:

1. Select ``Safety Island PSA Crypto APIs Architecture Test Suite``
   from the ``Use-Case`` menu.
2. Choose ``Save & Build``.

Run FVP
^^^^^^^

To start the FVP:

.. code-block:: console

  kas shell -c "../layers/meta-arm/scripts/runfvp -t tmux --verbose"

The ``PSA Crypto APIs Architecture Test Suite`` is deployed on all the 3 Safety
Island (SI) Clusters. The test result can be seen on the following tmux windows:

  * ``terminal_uart_si_cluster0``
  * ``terminal_uart_si_cluster1``
  * ``terminal_uart_si_cluster2``

The user can navigate through the panes by pressing ``Ctrl-b w`` and arrow keys
followed by the ``Enter`` key.

Run the tests
^^^^^^^^^^^^^

The tests will automatically run after the FVP is started. The complete test
suite takes around 8 minutes to complete. When the tests finish, a log similar
to the following should be visible. Normally no failure should be seen::

.. code-block:: console

  ************ Crypto Suite Report **********
  TOTAL TESTS     : 61
  TOTAL PASSED    : 61
  TOTAL SIM ERROR : 0
  TOTAL FAILED    : 0
  TOTAL SKIPPED   : 0
  ******************************************

Automated Validation
^^^^^^^^^^^^^^^^^^^^

To run the configuration menu:

.. code-block:: console

  kas menu kronos/Kconfig

To enable the validation tests:
  1. Select ``Safety Island PSA Crypto APIs Architecture Test Suite`` from the
     ``Use-Case`` menu.
  2. Choose ``Run Automated Validation`` from the ``Runtime Validation Setup``
     menu.
  3. Choose ``Save & Build``.

The complete test suite takes around 9 minutes to complete. See
:ref:`validation_psa_arch_tests` for more details. A similar output to the
following is printed out:

.. code-block:: console

  2023-11-21 07:08:11 - INFO     - RESULTS:
  2023-11-21 07:08:11 - INFO     - RESULTS - test_10_linuxlogin.LinuxLoginTest.test_linux_login: PASSED (20.98s)
  2023-11-21 07:08:11 - INFO     - RESULTS - test_10_si_psa_arch_tests.SIPSAArchTests.test_psa_si_cluster0: PASSED (233.00s)
  2023-11-21 07:08:11 - INFO     - RESULTS - test_10_si_psa_arch_tests.SIPSAArchTests.test_psa_si_cluster1: PASSED (0.01s)
  2023-11-21 07:08:11 - INFO     - RESULTS - test_10_si_psa_arch_tests.SIPSAArchTests.test_psa_si_cluster2: PASSED (0.01s)
  2023-11-21 07:08:11 - INFO     - RESULTS - test_00_lcp.LcpTest.test_normal_boot: PASSED (0.00s)
  2023-11-21 07:08:11 - INFO     - RESULTS - test_00_rss.RssTest.test_gic_multiple_view: PASSED (0.00s)
  2023-11-21 07:08:11 - INFO     - RESULTS - test_00_rss.RssTest.test_ni710ae: PASSED (0.00s)
  2023-11-21 07:08:11 - INFO     - RESULTS - test_00_rss.RssTest.test_normal_boot: PASSED (0.00s)
  2023-11-21 07:08:11 - INFO     - RESULTS - test_00_scp.ScpTest.test_normal_boot: PASSED (0.00s)
  2023-11-21 07:08:11 - INFO     - RESULTS - test_00_secure_partition.OpteeTest.test_optee_normal: PASSED (0.00s)
  2023-11-21 07:08:11 - INFO     - RESULTS - test_00_trusted_firmware_a.TrustedFirmwareTest.test_normal_boot: PASSED (0.00s)
  2023-11-21 07:08:11 - INFO     - RESULTS - test_10_linuxboot.LinuxBootTest.test_linux_boot: PASSED (284.33s)
  2023-11-21 07:08:11 - INFO     - SUMMARY:
  2023-11-21 07:08:11 - INFO     - baremetal-image () - Ran 12 tests in 538.328s

The following messages are expected to validate this Use-Case:

.. code-block:: console
    
  2023-11-21 07:08:11 - INFO     - RESULTS - test_10_si_psa_arch_tests.SIPSAArchTests.test_psa_si_cluster0: PASSED (233.00s)
  2023-11-21 07:08:11 - INFO     - RESULTS - test_10_si_psa_arch_tests.SIPSAArchTests.test_psa_si_cluster1: PASSED (0.01s)
  2023-11-21 07:08:11 - INFO     - RESULTS - test_10_si_psa_arch_tests.SIPSAArchTests.test_psa_si_cluster2: PASSED (0.01s)

|Arm SystemReadyTM| IR Validation
=================================

|Arm SystemReadyTM| IR Firmware Build
--------------------------------------

The Arm SystemReady IR Firmware Build option just builds the
|Arm SystemReadyTM| IR-aligned firmware. Refer to :ref:`design_systemready_ir`
for more details.

.. image:: ../images/kronos_reference_stack_build_config_sr_ir.png
   :align: center
   :width: 60 %

|

Build
^^^^^

To run the configuration menu:

.. code-block:: console

  kas menu kronos/Kconfig

To build the |Arm SystemReadyTM| IR-aligned firmware image:

1. Select ``Arm SystemReady IR Firmware Build`` under
   ``Arm SystemReady IR Validation`` from the ``Use-Case`` menu.
2. Then choose ``Save & Build``.

The firmware artifacts can be found in the directory
``build/tmp_systemready-glibc/deploy/images/fvp-rd-kronos/``.

.. _user_guide_reproduce_sr_ir_acs:

|Arm SystemReadyTM| IR Architecture Compliance Suite (ACS) Tests
----------------------------------------------------------------

The ACS for the |Arm SystemReadyTM| IR certification is delivered through a
live OS image, which enables the basic automation to run the tests.

The system will boot with the ACS live OS image and the ACS tests will run
automatically after the system boots. See :ref:`systemready_ir_acs_tests` for
more details.

Build and Automated Validation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To run the configuration menu:

.. code-block:: console

  kas menu kronos/Kconfig

To build and run the |Arm SystemReadyTM| IR ACS tests:

1. Select ``Arm SystemReady IR Architecture Compliance Suite (ACS) Tests`` under
   ``Arm SystemReady IR Validation`` from the ``Use-Case`` menu.
2. Then choose ``Save & Build``.

A similar output to the following is printed out:

.. code-block:: console

  2023-09-09 23:10:03 - INFO     - NOTE: recipe arm-systemready-ir-acs-1.0-r0: task do_testimage: Started
  2023-09-09 23:10:05 - INFO     - Creating terminal default on terminal_ns_uart0
  2023-09-09 23:10:16 - INFO     - Creating terminal tf-a on terminal_sec_uart
  2023-09-09 23:10:16 - INFO     - Creating terminal scp on terminal_uart_scp
  2023-09-09 23:10:16 - INFO     - Creating terminal lcp on terminal_uart_lcp
  2023-09-09 23:10:16 - INFO     - Creating terminal rss on terminal_rss_uart
  2023-09-09 23:10:16 - INFO     - Creating terminal safety_island_c0 on terminal_uart_si_cluster0
  2023-09-09 23:10:16 - INFO     - Creating terminal safety_island_c1 on terminal_uart_si_cluster1
  2023-09-09 23:10:17 - INFO     - Creating terminal safety_island_c2 on terminal_uart_si_cluster2
  2023-09-09 23:17:42 - INFO     - Test Group (PlatformSpecificElements): FAILED
  2023-09-09 23:18:47 - INFO     - Test Group (RequiredElements): FAILED
  2023-09-09 23:19:52 - INFO     - Test Group (CheckEvent_Conf): PASSED
  2023-09-09 23:21:03 - INFO     - Test Group (CheckEvent_Func): PASSED
  2023-09-09 23:22:07 - INFO     - Test Group (CloseEvent_Func): PASSED
  2023-09-09 23:23:13 - INFO     - Test Group (CreateEventEx_Conf): PASSED
  2023-09-09 23:24:15 - INFO     - Test Group (CreateEventEx_Func): PASSED
  2023-09-09 23:25:24 - INFO     - Test Group (CreateEvent_Conf): PASSED
  2023-09-09 23:26:26 - INFO     - Test Group (CreateEvent_Func): PASSED
  2023-09-09 23:27:29 - INFO     - Test Group (RaiseTPL_Func): PASSED
  2023-09-09 23:28:31 - INFO     - Test Group (RestoreTPL_Func): PASSED
  2023-09-09 23:29:33 - INFO     - Test Group (SetTimer_Conf): PASSED
  2023-09-09 23:37:17 - INFO     - Test Group (SetTimer_Func): PASSED
  2023-09-09 23:38:14 - INFO     - Test Group (SignalEvent_Func): PASSED
  2023-09-09 23:39:11 - INFO     - Test Group (WaitForEvent_Conf): PASSED
  2023-09-09 23:40:39 - INFO     - Test Group (WaitForEvent_Func): PASSED
  2023-09-09 23:41:36 - INFO     - Test Group (AllocatePages_Conf): PASSED
  2023-09-09 23:43:18 - INFO     - Test Group (AllocatePages_Func): PASSED
  2023-09-09 23:44:15 - INFO     - Test Group (AllocatePool_Conf): PASSED
  2023-09-09 23:45:14 - INFO     - Test Group (AllocatePool_Func): PASSED
  2023-09-09 23:46:11 - INFO     - Test Group (FreePages_Conf): PASSED
  2023-09-09 23:47:11 - INFO     - Test Group (FreePages_Func): PASSED
  2023-09-09 23:48:08 - INFO     - Test Group (GetMemoryMap_Conf): PASSED
  2023-09-09 23:49:06 - INFO     - Test Group (GetMemoryMap_Func): PASSED
  ...
  ...
  2023-09-10 08:13:13 - INFO     - Test Group (virtio_blk virtio1): vda
  2023-09-10 08:13:29 - INFO     - Linux tests complete
  2023-09-10 08:13:39 - INFO     - RESULTS:
  2023-09-10 08:13:39 - INFO     - RESULTS - arm_systemready_ir_acs.SystemReadyACSTest.test_acs: PASSED (32592.00s)
  2023-09-10 08:13:39 - INFO     - SUMMARY:
  2023-09-10 08:13:39 - INFO     - arm-systemready-ir-acs () - Ran 1 test in 32591.997s
  2023-09-10 08:13:39 - INFO     - arm-systemready-ir-acs - OK - All required tests passed (successes=1, skipped=0, failures=0, errors=0)

As seen in the above logs, some Test Groups are expected to fail. The following
messages are expected to validate this Use-Case:

.. code-block:: console

  2023-09-10 08:13:39 - INFO     - RESULTS - arm_systemready_ir_acs.SystemReadyACSTest.test_acs: PASSED (32592.00s)

.. note::

  The ACS tests take hours to complete. The actual time taken will vary
  depending on the performance of the build host. The default timeout setting
  for the tests is 12 hours for an x86_64 host or 24 hours for an aarch64 host.
  If a timeout failure occurs, please increase the timeout setting and re-run
  the tests with the following command on the build host terminal. The example
  command below changes the timeout setting to 16 hours.

  .. code-block:: shell

     TEST_OVERALL_TIMEOUT="\${@16*60*60}" kas shell -c "bitbake arm-systemready-ir-acs -C unpack"

Please refer to :ref:`systemready_ir_acs_tests` for an explanation on how the
ACS tests are set up and how they work in the Reference Stack.

.. _user_guide_reproduce_arm_systemready_ir_linux:

Linux Distribution Installation (Debian and openSUSE)
=====================================================

The |Arm SystemReadyTM| IR-aligned firmware must boot at least two unmodified
generic UEFI distribution images from an ISO image.

This Software Stack currently supports two Linux distributions: `Debian Stable`_
and `openSUSE Leap`_. To install Debian, you can refer to the
`Debian GNU/Linux Installation Guide`_. Similarly, you can refer to the
`openSUSE Installation Guide`_ for the installation of openSUSE.

.. note::

  The installation of a Linux distribution requires some manual interaction, for
  example, some necessary selections or confirmations, entering the user and
  password, etc.

  The whole installation process takes a long time (possibly up to 10 hours, or
  even longer).

  We suggest that when running the Linux distribution installations the FVP is
  the only running process as it will consume large amounts of RAM that can make
  the system unstable.

Please refer to :ref:`systemready_ir_linux_install` for an explanation on how
the Linux distros installation is set up and how they work in the Reference
Stack.

Debian
------

Distro Installation Media Preparation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To run the configuration menu:

.. code-block:: console

  kas menu kronos/Kconfig

To build the |Arm SystemReadyTM| IR Linux distros installation tests:

1. Choose ``Debian Linux Distro Installation`` under
   ``Linux Distribution Installation (Debian and openSUSE)`` from the
   ``Use-Case`` menu.
2. Then choose ``Save & Build``.

.. image:: ../images/kronos_reference_stack_build_config_sr_distro_debian.png
   :align: center
   :width: 60 %

|

Distro Installation
^^^^^^^^^^^^^^^^^^^

Run the following command to start the installation:

.. code-block:: console

  kas shell -c "../layers/meta-arm/scripts/runfvp -t tmux --verbose"


The whole process of installing Debian will probably take about 5 hours. The
install process begins when you see something like the below picture:

    .. image:: ../images/sr-ir-linux-distro-debian-install-grub-3.png
       :align: center
       :width: 60 %

The following are problems that have been encountered during the Debian
installation process and how to solve them:

* Detect and mount installation media

  1. After the installer starts, it will prompt
     ``No device for installation media was detected.`` in the
     ``Detect and mount installation media`` tab.
     Choose ``No`` to continue.

  .. image:: ../images/sr-ir-linux-distro-debian-install-media-0.png
     :align: center
     :width: 60 %

|

  2. Choose ``Yes`` to Manually select a module and device for installation
     media.

  .. image:: ../images/sr-ir-linux-distro-debian-install-media-1.png
     :align: center
     :width: 60 %

|

  3. Choose ``none`` to continue.

  .. image:: ../images/sr-ir-linux-distro-debian-install-media-2.png
     :align: center
     :width: 60 %

|

  4. Input ``/dev/mmcblk0`` as the device file for accessing the installation
     media.

  .. image:: ../images/sr-ir-linux-distro-debian-install-media-3.png
     :align: center
     :width: 60 %

|

* Install the GRUB boot loader

  When the installation reaches the ``Install the GRUB boot loader`` phase,
  there will be an error ``Unable to install GRUB in dummy``.
  This is because on EBBR platform, ``UEFI SetVariable()`` is not required at
  runtime (however, it is required at boot time), and Kronos happens to not
  support ``UEFI SetVariable()`` yet.

  .. image:: ../images/sr-ir-linux-distro-debian-install-grub-0.png
     :align: center
     :width: 60 %

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

  After doing the above GRUB workaround, press ``Ctrl-a p`` to go back to the
  installer again, then select ``Continue without boot loader`` in the
  ``Debian installer main menu`` and continue.

  .. image:: ../images/sr-ir-linux-distro-debian-install-grub-1.png
     :align: center
     :width: 60 %

|

* Finishing the installation

  When the installation reaches the final ``Finishing the installation``
  phase, you will need to wait some time to finish the remaining tasks,
  and then it will automatically reboot into the installed OS.

* Terminate the FVP

  Select the terminal titled as ``python3`` where the ``runfvp`` was launched
  by pressing ``Ctrl-b 0`` and press ``Ctrl-c`` to stop the FVP process.


openSUSE
--------

Distro Installation Media Preparation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To run the configuration menu:

.. code-block:: console

  kas menu kronos/Kconfig

To build the |Arm SystemReadyTM| IR Linux distros installation tests:

1. Choose ``openSUSE Linux Distro Installation`` under
   ``Linux Distribution Installation (Debian and openSUSE)`` from the
   ``Use-Case`` menu.
2. Then choose ``Save & Build``.

.. image:: ../images/kronos_reference_stack_build_config_sr_distro_opensuse.png
   :align: center
   :width: 60 %

|

Distro Installation
^^^^^^^^^^^^^^^^^^^

Run the following command to start the installation:

.. code-block:: console

  kas shell -c "../layers/meta-arm/scripts/runfvp -t tmux --verbose"

The whole process of installing openSUSE will take about 6 hours. Below are the
main steps and tips for installing openSUSE.

1. After the installer starts, select ``Installation`` to start installation
   process.

   .. image:: ../images/sr-ir-linux-distro-opensuse-install-installation.png
      :align: center
      :width: 60 %

2. It will take about 10 minutes to reach the ``Language, Keyboard and Licence
   Agreement`` tab. Select ``Next`` to continue.

   .. tip::

      Use ``Tab`` to cycle through options, and ``Enter`` to confirm.

3. After ``System Probing`` success, select ``No`` for ``Online Repositories``.

   .. image:: ../images/sr-ir-linux-distro-opensuse-install-online-repositories.png
      :align: center
      :width: 60 %

4. Select ``Server`` for ``System Role``, then select ``Next`` to continue.

   .. image:: ../images/sr-ir-linux-distro-opensuse-install-system-role.png
      :align: center
      :width: 60 %

5. Select ``Next`` to accept the ``Suggested Partitioning`` and continue.

   .. image:: ../images/sr-ir-linux-distro-opensuse-install-suggested-partitioning.png
      :align: center
      :width: 60 %

6. ``Create New User``, then select ``Next`` to continue.

   .. image:: ../images/sr-ir-linux-distro-opensuse-install-create-new-user.png
      :align: center
      :width: 60 %

7. If you're warned with ``The password is too simple``, it's fine to ignore and
   select ``Yes`` to continue.

   .. image:: ../images/sr-ir-linux-distro-opensuse-install-password-too-simple.png
      :align: center
      :width: 60 %

8. After ``Analyzing your system...``, a summary of installation settings will
   be given. Select ``Install`` to accept and continue.

   .. image:: ../images/sr-ir-linux-distro-opensuse-install-installation-settings.png
      :align: center
      :width: 60 %

9. Confirm Installation, select ``Install`` to continue.

   .. image:: ../images/sr-ir-linux-distro-opensuse-install-confirm-installation.png
      :align: center
      :width: 60 %

10. The installation will start after you select ``Install`` to continue, and it
    will take several hours. In the installation process,

    * ``Installing Packages...`` takes about 3 hours.
    * ``Save configuration`` takes about 5 minutes.
    * ``Save installation settings`` takes about 30 minutes.
    * ``Install boot manager`` takes about 20 minutes.
    * ``Prepare system for initial boot`` takes about 5 minutes.
    * Then the system will reboot automatically in 10s, you can select ``OK`` to
      reboot immediately.

    .. image:: ../images/sr-ir-linux-distro-opensuse-install-reboot.png
       :align: center
       :width: 60 %

11. The reboot process takes about 20 minutes. Then you can login the Linux
    shell with the user created in Step 6.

12. Select the terminal titled as ``python3`` where the ``runfvp`` was launched
    by pressing ``Ctrl-b 0`` and press ``Ctrl-c`` to stop the FVP process.

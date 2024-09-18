..
 # SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
 # affiliates <open-source-office@arm.com></text>
 #
 # SPDX-License-Identifier: MIT

#########
Reproduce
#########

This section of the User Guide describes how to download, configure, build and
execute the solutions in this repository.

************
Introduction
************

This repository uses the `kas menu tool`_ to configure and
customize the different :ref:`Use-Cases <overview_use_cases>` via a set of
configuration options provided in the configuration menu.

.. note::
  All command examples on this page from the HTML document format can be copied
  by clicking the copy button.
  In the PDF document format, be aware that special characters are added when
  lines get wrapped.

.. _user_guide_reproduce_environment_setup:

****************************
Build Host Environment Setup
****************************

.. _user_guide_reproduce_system_requirements:

System Requirements
===================

  * x86_64 or aarch64 host to build the stack and execute the FVP
  * Ubuntu Desktop or Server 20.04 Linux distribution
  * At least 500GiB of free disk for the download and builds
  * At least 32GiB of RAM memory
  * At least 12GiB of swap memory

.. _user_guide_reproduce_install_dependencies:

Install Dependencies
====================

  * Follow the Yocto Project documentation on
    `how to install the essential packages`_ required for the build host. The
    packages needed to build the Yocto Project documentation manuals are not
    required.

  * Install the kas tool and its optional dependency (to use the "menu" plugin):

    .. code-block:: text
      :substitutions:

      sudo -H pip3 install --upgrade kas==|kas version| && sudo apt install python3-newt

    For more details on kas installation, see
    `kas Dependencies & installation`_.
  * Install tmux and telnet (required for the ``runfvp`` tool):

    .. code-block:: text

      sudo apt install tmux telnet

.. _user_guide_reproduce_download:

********
Download
********

.. note::
  Performing the builds and FVP execution in **a tmux session is mandatory** for
  Arm Automotive Solutions because the ``runfvp`` tool that invokes the FVP expects
  the presence of a tmux session to attach its spawned tmux windows for console
  access to the processing elements.
  
  Refer to `Tmux Documentation`_ for more information on the usage of tmux.
  It is recommended to change the default ``history-limit`` by adding
  ``set-option -g history-limit 3000`` to ``~/.tmux.conf`` before starting
  tmux.

Start a new tmux session, via:

.. code-block:: text
  :substitutions:

  tmux new-session -s arm-auto-solutions

To reconnect to an existing tmux session:

.. code-block:: text
  :substitutions:

  tmux attach -t arm-auto-solutions

Download the Arm Automotive Solutions repository using Git and checkout a
release, via:

.. code-block:: text
  :substitutions:

  mkdir -p ~/arm-auto-solutions
  cd ~/arm-auto-solutions
  git clone |arm auto solutions remote| --branch |arm auto solutions version|

Upgrading from a previous version
=================================

When upgrading from a previous version of Arm Automotive Solutions, which may use a
different version of Yocto, you should run the following command to clean
the build directory before rebuilding the Use-Cases:

.. code-block:: text
  :substitutions:

  rm -rf build/cache build/tmp*

.. _user_guide_reproduce_use_cases:

*************************
Reproducing the Use-Cases
*************************

General
=======

Kas Build
---------

Arm Automotive Solutions has a kas configuration menu that can be used to build
the :ref:`overview_use_cases`. It can also apply customizable parameters to build
different Reference Software Stack Architecture types.

.. note::
  Before running the configuration menu, ensure it is done inside a tmux session.

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

.. note::
  To build and run any image for an Arms FVP the user has to accept its
  `EULA`_, which can be done by selecting the corresponding configuration
  option in the build setup. The Safety Island Actuation Demo is built as
  part of the default deployment.
  
  The kas build configuration menu selections performed in each use-case are
  saved. Ensure to only select the options mentioned in the use-case reproduce
  steps and deselect any other non-relevant ones.

.. image:: ../images/build_config.*
   :align: center
   :width: 60 %
   :alt: Arm Auto Solutions Build Configuration Menu

|

.. note::
  Typically, the build process should complete without any interruptions.
  However, if it is manually interrupted (e.g., by pressing ``Ctrl-c``) or due
  to network/resource failures, errors may occur when rerunning the build,
  such as:

  .. code-block:: text

    NOTE: Reconnecting to bitbake server...
    NOTE: No reply from server in 30s (for command <command> at 10:11:08.527092)

  This happens because some processes might still be running in the background.
  To resolve this, you can manually terminate them using: ``killall -e Cooker``
  
  Check for lock files and ensure there are no leftover lock files from the
  previous build. You can locate and remove them with:
  ``find . -name "bitbake.lock" -exec rm -f {} \;``
  
  If the above steps don’t resolve the issue, a system reboot might help clear
  any lingering problems.


FVP
---

The ``runfvp`` tool that invokes the FVP creates one tmux terminal
window per processing element. The default window displayed will be that of the
Primary Compute terminal titled as ``terminal_ns_uart0``. User may press
``Ctrl-b w`` to see the list of tmux windows and use arrow keys to navigate
through the windows and press ``Enter`` to select any processing element
terminal.

The Reference Software Stack running on the Primary Compute can be logged into
as ``root`` user without a password in the Linux terminal.

.. note::
  FVPs, and Fast Models in general, are functionally accurate, meaning that they
  fully execute all instructions correctly, however they are not cycle accurate.
  The main goal of the Reference Software Stack is to prove functionality only,
  and should not be used for performance analysis.

.. _user_guide_reproduce_cam:

Critical Application Monitoring Demo
====================================

The demo can be run on the Baremetal Architecture or Virtualization
Architecture. See :ref:`design_applications_cam` for further details.

Baremetal Architecture
----------------------

Build
^^^^^

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To build a Baremetal Architecture image:

1. Select ``Critical Application Monitoring Demo`` from the ``Use-Case`` menu.
2. Select ``Baremetal`` from the ``Reference Software Stack Architecture`` menu.
3. Select ``Build``.

Run the FVP
^^^^^^^^^^^

To start the FVP and connect to the Primary Compute terminal (running Linux):

.. code-block:: text

  kas shell -c "../layers/meta-arm/scripts/runfvp -t tmux --verbose"

The user should wait for the system to boot and for the Linux prompt to appear.

The Safety Island (SI) Cluster 1 terminal running ``cam-service`` is available
via the tmux window titled ``terminal_uart_si_cluster1``. For ease of
navigation, it's recommended to join the SI Cluster 1 terminal window to the
Primary Compute terminal window in order to issue commands on it.

Follow the steps below to achieve the same:

1. Ensure that the tmux window titled ``terminal_ns_uart0`` is selected. If not,
   press ``Ctrl-b w`` from the tmux session, navigate to the tmux window titled
   ``terminal_ns_uart0`` using the arrow keys, then press the ``Enter`` key.
2. Press ``Ctrl-b :`` and then type
   ``join-pane -s :terminal_uart_si_cluster1 -h`` followed by pressing ``Enter``
   key to join the SI Cluster 1 terminal window to the Primary Compute
   terminal window.

Refer to the following image of the tmux panes rearrangement. Panes can
be navigated using ``Ctrl-b`` followed by the arrow keys.

.. image:: ../images/cam_reproduce_startup.*
  :align: center
  :alt: Critical Application Monitoring Demo Reproduce Startup

|

The Reference Software Stack running on the Primary Compute can be logged into
as ``root`` user without a password in the Linux terminal. Run the below
command to guarantee that all the expected services have been
initialized.

.. code-block:: text

  systemctl is-system-running --wait

Wait for it to return. The expected terminal output is ``running``.

Run the Demo
^^^^^^^^^^^^

Before running ``cam-app-example``, ``.csd`` files corresponding to event
streams produced by ``cam-app-example`` must be created and deployed to the
system where ``cam-service`` runs (in this case SI Cluster 1). Run
``cam-app-example`` in calibration mode and then use ``cam-tool`` to generate
the ``.csd`` files.

1. Start ``cam-app-example`` in calibration mode from the Primary Compute
   terminal:

   .. code-block:: text

      cam-app-example -u 11085ddc-bc10-11ed-9a44-7ef9696e -t 3000 -c 10 -s 4 -C

   The stream event log files (``.csel``) for each stream are generated. The output
   should look as below:

   .. code-block:: text

      Cam application configuration:
          Service IP address: 127.0.0.1
          Service port: 21604
          UUID base: 11085ddc-bc10-11ed-9a44-7ef9696e
          Stream count: 4
          Processing period (ms): 3000
          Processing count: 10
          Multiple connection support: false
          Calibration mode support: true
          Calibration directory: ./[uuid].csel
          Fault injection support: false
          Event(s) interval time (ms): 0
      Using libcam v1.0
      Starting activity...
      Starting activity...
      Starting activity...
      Starting activity...
          Stream 0 sends event 0
          Stream 1 sends event 0
          Stream 2 sends event 0
          Stream 3 sends event 0
          ...

   List the files generated:

   .. code-block:: text

      ls -1 *.csel

   The stream event log files can be shown as below:

   .. code-block:: text

      11085ddc-bc10-11ed-9a44-7ef9696e0000.csel
      11085ddc-bc10-11ed-9a44-7ef9696e0001.csel
      11085ddc-bc10-11ed-9a44-7ef9696e0002.csel
      11085ddc-bc10-11ed-9a44-7ef9696e0003.csel

2. Run ``cam-tool`` from the Primary Compute terminal to analyze stream event
   log files and convert them to stream configuration files (``.csc.yml``).

   .. code-block:: text

      cam-tool analyze -m 1000000 -i 11085ddc-bc10-11ed-9a44-7ef9696e0000.csel

   The analysis result is reported from the Primary Compute terminal as below,
   the timeout value might change:

   .. code-block:: text

      CAM event log analyze report:
      Input event log file:                   11085ddc-bc10-11ed-9a44-7ef9696e0000.csel
      Output configuration file:              analyzed.csc.yml
      Stream UUID:                            11085ddc-bc10-11ed-9a44-7ef9696e0000
      Stream name:                            CAM STREAM  0
      Timeout between init and start:         300000
      Timeout between start and event:        450000
      Application running times:              1
      Processing count in each run:           [10]

      Event ID        timeout
      0               4000106

   The stream configuration files contain human-readable settings used for the
   deployment phase of a critical application. Users can modify this
   configuration, for example to fine tune timeout values depending on the
   system capabilities.

   Run ``cam-tool`` three more times for each of the other three streams.

   .. code-block:: text

      cam-tool analyze -m 1000000 -i 11085ddc-bc10-11ed-9a44-7ef9696e0001.csel
      cam-tool analyze -m 1000000 -i 11085ddc-bc10-11ed-9a44-7ef9696e0002.csel
      cam-tool analyze -m 1000000 -i 11085ddc-bc10-11ed-9a44-7ef9696e0003.csel

   Then, use the ``cam-tool pack`` command for each of the streams to generate
   deployment data.

   .. code-block:: text

      cam-tool pack -i 11085ddc-bc10-11ed-9a44-7ef9696e0000.csc.yml
      cam-tool pack -i 11085ddc-bc10-11ed-9a44-7ef9696e0001.csc.yml
      cam-tool pack -i 11085ddc-bc10-11ed-9a44-7ef9696e0002.csc.yml
      cam-tool pack -i 11085ddc-bc10-11ed-9a44-7ef9696e0003.csc.yml

3. Run the ``cam-tool deploy`` command from the Primary Compute terminal to
   transfer the generated stream deployment data to SI Cluster 1 (where
   ``cam-service`` is running):

   .. code-block:: text

      cam-tool deploy -i 11085ddc-bc10-11ed-9a44-7ef9696e0000.csd -a 192.168.1.1

   The output on the SI Cluster 1 terminal should look as below, the
   connection number might change:

   .. code-block:: text

      Connection 4 is created.
      Deploy Message

      Connection 4 is closed.

   After that, the stream data of ``11085ddc-bc10-11ed-9a44-7ef9696e0000`` is
   deployed to the ``cam-service`` file system.

   Running ``cam-tool deploy`` three more times can deploy the data of three
   other streams to ``cam-service``.

   .. code-block:: text

      cam-tool deploy -i 11085ddc-bc10-11ed-9a44-7ef9696e0001.csd -a 192.168.1.1
      cam-tool deploy -i 11085ddc-bc10-11ed-9a44-7ef9696e0002.csd -a 192.168.1.1
      cam-tool deploy -i 11085ddc-bc10-11ed-9a44-7ef9696e0003.csd -a 192.168.1.1

   The output on the SI Cluster 1 terminal after each one of the
   ``cam-tool deploy`` command should look as below, the connection number
   might change:

   .. code-block:: text

      Connection 4 is created.
      Deploy Message

      Connection 4 is closed.

   List all the files from the SI Cluster 1 terminal:

   .. code-block:: text

      fs ls RAM:/

   The stream deployment data can be shown as below:

   .. code-block:: text

      11085ddc-bc10-11ed-9a44-7ef9696e0000.csd
      11085ddc-bc10-11ed-9a44-7ef9696e0001.csd
      11085ddc-bc10-11ed-9a44-7ef9696e0002.csd
      11085ddc-bc10-11ed-9a44-7ef9696e0003.csd

4. Start ``cam-app-example`` from the Primary Compute terminal to create an
   application with four streams. Each stream sends an event message 10 times
   with a period of 3000 milliseconds.

   .. code-block:: text

      cam-app-example -u 11085ddc-bc10-11ed-9a44-7ef9696e -t 3000 -c 10 -s 4 -a 192.168.1.1

   The following configure messages are expected from the Primary Compute
   terminal:

   .. code-block:: text

      Cam application configuration:
          Service IP address: 192.168.1.1
          Service port: 21604
          UUID base: 11085ddc-bc10-11ed-9a44-7ef9696e
          Stream count: 4
          Processing period (ms): 3000
          Processing count: 10
          Multiple connection support: false
          Calibration mode support: false
          Fault injection support: false
          Event(s) interval time (ms): 0
      Using libcam v1.0
      Starting activity...
      Starting activity...
      Starting activity...
      Starting activity...

   And the log of sent event messages are shown repeatedly:

   .. code-block:: text

    Stream 0 sends event 0
    Stream 1 sends event 0
    Stream 2 sends event 0
    Stream 3 sends event 0
    Stream 0 sends event 0
    Stream 1 sends event 0
    Stream 2 sends event 0
    Stream 3 sends event 0
    ...

   As observed from the SI Cluster 1 terminal, ``cam-service`` is loading
   four stream deployment files for monitoring. In the following log, the stream
   messages are received and processed by it:

   .. code-block:: text

      Connection 4 is created.
      Init Message
      Stream 11085ddc-bc10-11ed-9a44-7ef9696e0001 configuration is loaded.
      Init Message
      Stream 11085ddc-bc10-11ed-9a44-7ef9696e0000 configuration is loaded.
      Init Message
      Stream 11085ddc-bc10-11ed-9a44-7ef9696e0002 configuration is loaded.
      Init Message
      Stream 11085ddc-bc10-11ed-9a44-7ef9696e0003 configuration is loaded.
      Start Message
      Start Message
      Start Message
      Start Message
      Event Message
      Event Message
      Event Message
      Event Message
      Event Message
      # Repeated event messages
      ...

5. ``cam-app-example`` has a mode to inject a fault to test the CAM framework.
   Run ``cam-app-example`` again from the Primary Compute terminal with fault
   injection to event stream 0:

   .. code-block:: text

      cam-app-example -u 11085ddc-bc10-11ed-9a44-7ef9696e -t 3000 -c 10 -s 4 -f -S 0 -T 1000 -a 192.168.1.1

   The following configure messages are expected from the Primary Compute
   terminal:

   .. code-block:: text

      Cam application configuration:
          Service IP address: 192.168.1.1
          Service port: 21604
          UUID base: 111085ddc-bc10-11ed-9a44-7ef9696e
          Stream count: 4
          Processing period (ms): 3000
          Processing count: 10
          Multiple connection support: false
          Calibration mode support: false
          Fault injection support: true
          Fault injection time: 1000
          Fault injection stream: 0
          Event(s) interval time (ms): 0
      Using libcam v1.0
      Starting activity...
      Starting activity...
      Starting activity...
      Starting activity...

   And the log of sent event messages are shown repeatedly:

   .. code-block:: text

    Stream 0 sends event 0
    Stream 1 sends event 0
    Stream 2 sends event 0
    Stream 3 sends event 0
    Stream 0 sends event 0
    Stream 1 sends event 0
    Stream 2 sends event 0
    Stream 3 sends event 0
    ...

   The fault happens 1000ms after stream initialization. At that time
   ``cam-service`` should detect a stream temporal error with the following
   output from the SI Cluster 1 terminal.

   .. code-block:: text

      # Repeated event messages
      ...
      ERROR: Stream temporal error:
      ERROR:     stream_name: CAM STREAM  0
      ERROR:     stream_uuid: 11085ddc-bc10-11ed-9a44-7ef9696e0000
      ERROR:     event_id: 0
      ERROR:     time_received: 0
      ERROR:     time_expected: 1710328901375511
      # Repeated event messages
      ...
      ERROR: Stream state error:
      ERROR:     stream_name: CAM STREAM  0
      ERROR:     stream_uuid: 11085ddc-bc10-11ed-9a44-7ef9696e0000
      ERROR:     timestamp: 1710328927375278
      ERROR:     current_state: Failed state
      ERROR:     requested_state: In-progress state

.. note::
   ``time_received: 0`` should be ignored as the ``time_received`` is not set
   during a fault.

6. To shut down the FVP and terminate the emulation automatically, issue the
   following command on the Primary Compute terminal.

   .. code-block:: text

      shutdown now

   The below messages indicate the shutdown process is complete.

   .. code-block:: text

      [  OK  ] Finished System Power Off.
      [  OK  ] Reached target System Power Off.
      reboot: Power down


Automated Validation
^^^^^^^^^^^^^^^^^^^^

Run this step to ensure the creation of the initial firmware flash images:


.. code-block:: text

  kas shell -c "bitbake firmware-fvp-rd-kronos -C deploy"

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To run the validation tests:

  1. Select ``Critical Application Monitoring Demo`` as ``Use-Case``.
  2. Select ``Baremetal`` from the ``Reference Software Stack Architecture``
     menu.
  3. Select ``Run Automated Validation`` from the ``Runtime Validation Setup``
     menu.
  4. Select ``Build``.

The complete test suite takes around 40 minutes to complete. See
:ref:`validation_cam_tests` for more details.

The following messages are expected in the output to validate this Use-Case:

.. code-block:: text

   RESULTS - test_40_cam.CAMServiceTest.test_cam_service_boot_on_si: PASSED (0.00s)
   RESULTS - test_40_cam.CAMTest.test_cam_app_example_to_service_on_si: PASSED (36.89s)
   RESULTS - test_40_cam.CAMTest.test_cam_app_example_to_service_on_si_with_multiple_connections: PASSED (36.98s)
   RESULTS - test_40_cam.CAMTest.test_cam_ptp_sync: PASSED (0.00s)
   RESULTS - test_40_cam.CAMTest.test_cam_tool_deploy_to_si: PASSED (75.69s)
   RESULTS - test_40_cam.CAMTest.test_cam_tool_pack: PASSED (74.72s)
   RESULTS - test_40_cam.CAMTest.test_data_calibration: PASSED (187.73s)
   RESULTS - test_40_cam.CAMTest.test_logical_check_on_si: PASSED (15.63s)
   RESULTS - test_40_cam.CAMTest.test_temporal_check_on_si: PASSED (44.32s)

.. note::
    There is a rare known failure where a timeout might occur during test execution. 
    
    Refer to :ref:`releasenotes_knownissues` for possible workarounds.

Virtualization Architecture
---------------------------

Build
^^^^^

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To build a Virtualization Architecture image:

1. Select ``Critical Application Monitoring Demo`` from the ``Use-Case`` menu.
2. Select ``Virtualization`` from the ``Reference Software Stack Architecture``
   menu.
3. Select ``Build``.

Run the FVP
^^^^^^^^^^^

To start the FVP and connect to the Primary Compute terminal (running Linux):

.. code-block:: text

  kas shell -c "../layers/meta-arm/scripts/runfvp -t tmux --verbose"

The user should wait for the system to boot and for the Linux prompt to appear.

The Safety Island (SI) Cluster 1 terminal running ``cam-service`` is available
via the tmux window titled ``terminal_uart_si_cluster1``. For ease of
navigation, it's recommended to join the SI Cluster 1 terminal window to the
Primary Compute terminal window in order to issue commands on it.

Follow the steps below to achieve the same:

1. Ensure that the tmux window titled ``terminal_ns_uart0`` is selected. If not,
   press ``Ctrl-b w`` from the tmux session, navigate to the tmux window titled
   ``terminal_ns_uart0`` using the arrow keys, then press the ``Enter`` key.
2. Press ``Ctrl-b :`` and then type
   ``join-pane -s :terminal_uart_si_cluster1 -h`` followed by pressing ``Enter``
   key to join the SI Cluster 1 terminal window to the Primary Compute
   terminal window.

Since both DomU1 and DomU2 will be used to run ``cam-app-example``, it is also
recommended to create a tmux pane to connect to DomU2.

1. Press ``Ctrl-b`` and the arrow keys to navigate to the ``terminal_ns_uart0``
   pane.
2. Press ``Ctrl-b "`` to split the pane horizontally. The bottom pane will be
   used to connect to DomU2.

Please refer to the following image of the tmux panes rearrangement. Panes can
be navigated using ``Ctrl-b`` followed by the arrow keys.

.. image:: ../images/cam_reproduce_startup_virtualization.*
  :align: center
  :alt: Critical Application Monitoring Demo Reproduce Startup for Virtualization

|

The Reference Software Stack running on the Primary Compute can be logged into
as ``root`` user without a password in the Linux terminal. Run the below
command to guarantee that all the expected services have been
initialized.

.. code-block:: text

  systemctl is-system-running --wait

Wait for it to return. The expected terminal output is ``running``.

.. note::
  A message similar to the following might appear in the Primary Compute
  terminal ``(XEN) d2v0: vGICR: SGI: unhandled word write 0x000000ffffffff
  to ICACTIVER0``, this is an expected behavior.

Run the Demo
^^^^^^^^^^^^

Before running ``cam-app-example``, ``.csd`` files corresponding to event
streams produced by ``cam-app-example`` must be created and deployed to the
system where ``cam-service`` runs (in this case SI Cluster 1). Run
``cam-app-example`` in calibration mode and then use ``cam-tool`` to generate
the ``.csd`` files.

1. From the Primary Compute terminal, enter the DomU1 console using the ``xl``
   tool:

   .. code-block:: text

      xl console domu1

   DomU1 can be logged into as ``root`` user without a password in the Linux
   terminal. This command will provide a console on the DomU1. To exit,
   enter ``Ctrl-]`` (to access the FVP telnet shell), followed by typing
   ``send esc`` into the telnet shell and pressing ``Enter``. See the
   `xl documentation`_ for further details.

2. To improve the readability of commands and output on the DomU1 console, run
   the command below:

   .. code-block:: text

      stty rows 76 cols 282

3. From the host terminal, SSH to the FVP then enter the DomU2 console using the
   ``xl`` tool:

   .. code-block:: text

      ssh root@127.0.0.1 -p 2222
      xl console domu2

   DomU2 can be logged into as ``root`` user without a password in the Linux
   terminal. This command will provide a console on the DomU2. To exit,
   enter ``Ctrl-]`` (to access the FVP telnet shell), followed by typing
   ``send esc`` into the telnet shell and pressing ``Enter``. See the
   `xl documentation`_ for further details.

4. To improve the readability of commands and output on the DomU2 console, run
   the command below:

   .. code-block:: text

      stty rows 76 cols 282

5. From the DomU1 terminal, check that the clock is synchronized using the
   command ``timedatectl``, one of the line of its output needs to be
   ``System clock synchronized: yes`` to confirm that the clock is synchronized:

   .. code-block:: text

      timedatectl

   The output should look as below, the date and time can differ, in case the
   ``System clock synchronized:`` shows the ``no`` value, allow at least 1 minute
   for the system to settle and for the clock to synchronize, afterwards repeat
   the step 5 until ``System clock synchronized: yes`` is shown in the output:

   .. code-block:: text

                     Local time: Thu 2024-03-14 12:56:26 UTC
                 Universal time: Thu 2024-03-14 12:56:26 UTC
                       RTC time: n/a
                      Time zone: UTC (UTC, +0000)
      System clock synchronized: yes
                    NTP service: n/a
                RTC in local TZ: no

6. Start ``cam-app-example`` in calibration mode from the DomU1 terminal:

   .. code-block:: text

      cam-app-example -u 11085ddc-bc10-11ed-9a44-7ef9696e -t 3000 -c 10 -s 4 -C

   The stream event log files (``.csel``) for each stream are generated. The output
   should look as below:

   .. code-block:: text

      Cam application configuration:
          Service IP address: 127.0.0.1
          Service port: 21604
          UUID base: 11085ddc-bc10-11ed-9a44-7ef9696e
          Stream count: 4
          Processing period (ms): 3000
          Processing count: 10
          Multiple connection support: false
          Calibration mode support: true
          Calibration directory: ./[uuid].csel
          Fault injection support: false
          Event(s) interval time (ms): 0
      Using libcam v1.0
      Starting activity...
      Starting activity...
      Starting activity...
      Starting activity...
          Stream 0 sends event 0
          Stream 1 sends event 0
          Stream 2 sends event 0
          Stream 3 sends event 0
          ...

   List the files generated:

   .. code-block:: text

      ls -1 *.csel

   The stream event log files can be shown as below:

   .. code-block:: text

      11085ddc-bc10-11ed-9a44-7ef9696e0000.csel
      11085ddc-bc10-11ed-9a44-7ef9696e0001.csel
      11085ddc-bc10-11ed-9a44-7ef9696e0002.csel
      11085ddc-bc10-11ed-9a44-7ef9696e0003.csel

7. Run ``cam-tool`` from the DomU1 terminal to analyze stream event log files
   and convert them to stream configuration files (``.csc.yml``).

   .. code-block:: text

      cam-tool analyze -m 1000000 -i 11085ddc-bc10-11ed-9a44-7ef9696e0000.csel

   The analysis result is reported from the DomU1 terminal as below,
   the timeout value might change:

   .. code-block:: text

      CAM event log analyze report:
      Input event log file:                   11085ddc-bc10-11ed-9a44-7ef9696e0000.csel
      Output configuration file:              analyzed.csc.yml
      Stream UUID:                            11085ddc-bc10-11ed-9a44-7ef9696e0000
      Stream name:                            CAM STREAM  0
      Timeout between init and start:         300000
      Timeout between start and event:        450000
      Application running times:              1
      Processing count in each run:           [10]

      Event ID        timeout
      0               4000072

   The stream configuration files contain human-readable settings used for the
   deployment phase of a critical application. Users can modify this
   configuration, for example to fine tune timeout values depending on the
   system capabilities.

   Run ``cam-tool`` three more times for each of the other three streams.

   .. code-block:: text

      cam-tool analyze -m 1000000 -i 11085ddc-bc10-11ed-9a44-7ef9696e0001.csel

   .. code-block:: text

      cam-tool analyze -m 1000000 -i 11085ddc-bc10-11ed-9a44-7ef9696e0002.csel

   .. code-block:: text

      cam-tool analyze -m 1000000 -i 11085ddc-bc10-11ed-9a44-7ef9696e0003.csel

   Then, use the ``cam-tool pack`` command for each of the streams to generate
   deployment data.

   .. code-block:: text

      cam-tool pack -i 11085ddc-bc10-11ed-9a44-7ef9696e0000.csc.yml

   .. code-block:: text

      cam-tool pack -i 11085ddc-bc10-11ed-9a44-7ef9696e0001.csc.yml

   .. code-block:: text

      cam-tool pack -i 11085ddc-bc10-11ed-9a44-7ef9696e0002.csc.yml

   .. code-block:: text

      cam-tool pack -i 11085ddc-bc10-11ed-9a44-7ef9696e0003.csc.yml

8. Run the ``cam-tool deploy`` command from the DomU1 terminal to
   transfer the generated stream deployment data to SI Cluster 1 (where
   ``cam-service`` is running):

   .. code-block:: text

      cam-tool deploy -i 11085ddc-bc10-11ed-9a44-7ef9696e0000.csd -a 192.168.1.1

   The output on the SI Cluster 1 terminal should look as below, the
   connection number might change:

   .. code-block:: text

      Connection 4 is created.
      Deploy Message

      Connection 4 is closed.

   After that, the stream data of ``11085ddc-bc10-11ed-9a44-7ef9696e0000`` is
   deployed to the ``cam-service`` file system.

   Running ``cam-tool deploy`` three more times can deploy the data of three
   other streams to ``cam-service``.

   .. code-block:: text

      cam-tool deploy -i 11085ddc-bc10-11ed-9a44-7ef9696e0001.csd -a 192.168.1.1

   .. code-block:: text

      cam-tool deploy -i 11085ddc-bc10-11ed-9a44-7ef9696e0002.csd -a 192.168.1.1

   .. code-block:: text

      cam-tool deploy -i 11085ddc-bc10-11ed-9a44-7ef9696e0003.csd -a 192.168.1.1

   The output on the SI Cluster 1 terminal after each one of the
   ``cam-tool deploy`` command should look as below, the connection number
   might change:

   .. code-block:: text

      Connection 4 is created.
      Deploy Message

      Connection 4 is closed.

9. From the DomU2 terminal, check that the clock is synchronized using the
   command ``timedatectl``, one of the line of its output needs to be
   ``System clock synchronized: yes`` to confirm that the clock is synchronized:

   .. code-block:: text

      timedatectl

   The output should look as below, the date and time can differ, in case the
   ``System clock synchronized:`` shows the ``no`` value, allow at least 1 minute
   for the system to settle and for the clock to synchronize, afterwards repeat
   the step 9 until ``System clock synchronized: yes`` is shown in the output:

   .. code-block:: text

                     Local time: Thu 2024-03-14 12:56:26 UTC
                 Universal time: Thu 2024-03-14 12:56:26 UTC
                       RTC time: n/a
                      Time zone: UTC (UTC, +0000)
      System clock synchronized: yes
                    NTP service: n/a
                RTC in local TZ: no

10. Start ``cam-app-example`` in calibration mode from the DomU2 terminal:

    .. code-block:: text

       cam-app-example -u 22085ddc-bc10-11ed-9a44-7ef9696e -t 2000 -c 5 -s 2 -C

    The stream event log files (``.csel``) for each stream are generated. The output
    should look as below:

    .. code-block:: text

       Cam application configuration:
           Service IP address: 127.0.0.1
           Service port: 21604
           UUID base: 22085ddc-bc10-11ed-9a44-7ef9696e
           Stream count: 2
           Processing period (ms): 2000
           Processing count: 5
           Multiple connection support: false
           Calibration mode support: true
           Calibration directory: ./[uuid].csel
           Fault injection support: false
           Event(s) interval time (ms): 0
       Using libcam v1.0
       Starting activity...
       Starting activity...
           Stream 0 sends event 0
           Stream 1 sends event 0
           ...

    List the files generated:

    .. code-block:: text

       ls -1 *.csel

    The stream event log files can be shown as below:

    .. code-block:: text

       22085ddc-bc10-11ed-9a44-7ef9696e0000.csel
       22085ddc-bc10-11ed-9a44-7ef9696e0001.csel

11. Run ``cam-tool`` from the DomU2 terminal to analyze stream event log files
    and convert them to stream configuration files (``.csc.yml``).

    .. code-block:: text

       cam-tool analyze -m 1000000 -i 22085ddc-bc10-11ed-9a44-7ef9696e0000.csel

    The analysis result is reported from the DomU2 terminal as below,
    the timeout value might change:

    .. code-block:: text

       CAM event log analyze report:
       Input event log file:                   22085ddc-bc10-11ed-9a44-7ef9696e0000.csel
       Output configuration file:              analyzed.csc.yml
       Stream UUID:                            22085ddc-bc10-11ed-9a44-7ef9696e0000
       Stream name:                            CAM STREAM  0
       Timeout between init and start:         300000
       Timeout between start and event:        450000
       Application running times:              1
       Processing count in each run:           [5]

       Event ID        timeout
       0               3000001

    The stream configuration files contain human-readable settings used for the
    deployment phase of a critical application. Users can modify this
    configuration, for example to fine tune timeout values depending on the
    system capabilities.

    Run ``cam-tool`` for the other stream.

    .. code-block:: text

       cam-tool analyze -m 1000000 -i 22085ddc-bc10-11ed-9a44-7ef9696e0001.csel

    Then, use the ``cam-tool pack`` command for each of the streams to generate
    deployment data.

    .. code-block:: text

       cam-tool pack -i 22085ddc-bc10-11ed-9a44-7ef9696e0000.csc.yml
       cam-tool pack -i 22085ddc-bc10-11ed-9a44-7ef9696e0001.csc.yml

12. Run the ``cam-tool deploy`` command from the DomU2 terminal to
    transfer the generated stream deployment data to SI Cluster 1 (where
    ``cam-service`` is running):

    .. code-block:: text

       cam-tool deploy -i 22085ddc-bc10-11ed-9a44-7ef9696e0000.csd -a 192.168.1.1

    The output on the SI Cluster 1 terminal should look as below, the
    connection number might change:

    .. code-block:: text

       Connection 4 is created.
       Deploy Message

       Connection 4 is closed.

    After that, the stream data of ``22085ddc-bc10-11ed-9a44-7ef9696e0000`` is
    deployed to the ``cam-service`` file system.

    Running ``cam-tool deploy`` one more time deploys the data of the other
    stream to ``cam-service``.

    .. code-block:: text

       cam-tool deploy -i 22085ddc-bc10-11ed-9a44-7ef9696e0001.csd -a 192.168.1.1

    The output on the SI Cluster 1 terminal after each one of the
    ``cam-tool deploy`` command should look as below, the connection number
    might change:

    .. code-block:: text

       Connection 4 is created.
       Deploy Message

       Connection 4 is closed.

13. List all the files from the SI Cluster 1 terminal:

    .. code-block:: text

       fs ls RAM:/

    The stream deployment data can be shown as below:

    .. code-block:: text

       11085ddc-bc10-11ed-9a44-7ef9696e0000.csd
       11085ddc-bc10-11ed-9a44-7ef9696e0001.csd
       11085ddc-bc10-11ed-9a44-7ef9696e0002.csd
       11085ddc-bc10-11ed-9a44-7ef9696e0003.csd
       22085ddc-bc10-11ed-9a44-7ef9696e0000.csd
       22085ddc-bc10-11ed-9a44-7ef9696e0001.csd

14. Start ``cam-app-example`` from the DomU1 terminal to create an
    application with four streams. Each stream sends an event message 10 times
    with a period of 3000 milliseconds.

    .. code-block:: text

       cam-app-example -u 11085ddc-bc10-11ed-9a44-7ef9696e -t 3000 -c 10 -s 4 -a 192.168.1.1

    The following configure messages are expected from the Primary Compute
    terminal:

    .. code-block:: text

       Cam application configuration:
           Service IP address: 192.168.1.1
           Service port: 21604
           UUID base: 11085ddc-bc10-11ed-9a44-7ef9696e
           Stream count: 4
           Processing period (ms): 3000
           Processing count: 10
           Multiple connection support: false
           Calibration mode support: false
           Fault injection support: false
           Event(s) interval time (ms): 0
       Using libcam v1.0
       Starting activity...
       Starting activity...
       Starting activity...
       Starting activity...

    And the log of sent event messages are shown repeatedly:

    .. code-block:: text

     Stream 0 sends event 0
     Stream 1 sends event 0
     Stream 2 sends event 0
     Stream 3 sends event 0
     Stream 0 sends event 0
     Stream 1 sends event 0
     Stream 2 sends event 0
     Stream 3 sends event 0
     ...

    While ``cam-app-example`` is running on DomU1, start another instance on
    DomU2. ``cam-app-example`` has a mode to inject a fault to test the CAM
    framework. Run ``cam-app-example`` again from the DomU2 terminal with fault
    injection to event stream 0:

    .. code-block:: text

       cam-app-example -u 22085ddc-bc10-11ed-9a44-7ef9696e -t 2000 -c 5 -s 2 -f -S 0 -T 1000 -a 192.168.1.1

   The following configure messages are expected from the Primary Compute
   terminal:

   .. code-block:: text

      Cam application configuration:
          Service IP address: 192.168.1.1
          Service port: 21604
          UUID base: 22085ddc-bc10-11ed-9a44-7ef9696e
          Stream count: 2
          Processing period (ms): 2000
          Processing count: 5
          Multiple connection support: false
          Calibration mode support: false
          Fault injection support: true
          Fault injection time: 1000
          Fault injection stream: 0
          Event(s) interval time (ms): 0
      Using libcam v1.0
      Starting activity...
      Starting activity...

   And the log of sent event messages are shown repeatedly:

   .. code-block:: text

    Stream 0 sends event 0
    Stream 1 sends event 0
    Stream 1 sends event 0
    Stream 1 sends event 0
    ...

    As observed from the SI Cluster 1 terminal, ``cam-service`` is loading
    four stream deployment files from DomU1 and two stream deployment files from
    DomU2 for monitoring. In the following log, the stream messages are received
    and processed by it:

    .. code-block:: text

       Connection 4 is created.
       Init Message
       Stream 11085ddc-bc10-11ed-9a44-7ef9696e0002 configuration is loaded.
       Init Message
       Stream 11085ddc-bc10-11ed-9a44-7ef9696e0001 configuration is loaded.
       Init Message
       Stream 11085ddc-bc10-11ed-9a44-7ef9696e0003 configuration is loaded.
       Init Message
       Stream 11085ddc-bc10-11ed-9a44-7ef9696e0000 configuration is loaded.
       Start Message
       Start Message
       Start Message
       Start Message
       Event Message
       Event Message
       Event Message
       Event Message

       Connection 5 is created.
       Init Message
       Stream 22085ddc-bc10-11ed-9a44-7ef9696e0001 configuration is loaded.
       Init Message
       Stream 22085ddc-bc10-11ed-9a44-7ef9696e0000 configuration is loaded.
       Start Message
       Start Message
       Event Message
       Event Message
       Event Message
       # Repeated event messages
       ...

    The fault happens 1000ms after stream initialization. At that time
    ``cam-service`` should detect a stream temporal error with the following
    output from the SI Cluster 1 terminal.

    .. code-block:: text

       # Repeated event messages
       ...
       ERROR: Stream temporal error:
       ERROR:     stream_name: CAM STREAM  0
       ERROR:     stream_uuid: 2285ddc-bc10-11ed-9a44-7ef9696e0000
       ERROR:     event_id: 0
       ERROR:     time_received: 0
       ERROR:     time_expected: 1710275907816057
       # Repeated event messages
       ...
       ERROR: Stream state error:
       ERROR:     stream_name: CAM STREAM  0
       ERROR:     stream_uuid: 22085ddc-bc10-11ed-9a44-7ef9696e0000
       ERROR:     timestamp: 1710275909816069
       ERROR:     current_state: Failed state
       ERROR:     requested_state: In-progress state

.. note::
   ``time_received: 0`` should be ignored as the ``time_received`` is not set
   during a fault.

15. To leave the DomU1 console, type ``Ctrl-]`` and enter ``send esc``.

16. To leave the DomU2 console, type ``Ctrl-]``.

17. To shut down the FVP and terminate the emulation automatically, follow the
    below steps:

    * Issue the following command on the Primary Compute terminal.

      .. code-block:: text

         shutdown now

      The below messages indicate the shutdown process is complete.

      .. code-block:: text

         [  OK  ] Finished System Power Off.
         [  OK  ] Reached target System Power Off.
         reboot: Power down

    * Close the tmux pane started for DomU2 by pressing ``Ctrl-d``.


Automated Validation
^^^^^^^^^^^^^^^^^^^^

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To run the validation tests:

  1. Select ``Critical Application Monitoring Demo`` as ``Use-Case``.
  2. Select ``Virtualization`` from the
     ``Reference Software Stack Architecture`` menu.
  3. Select ``Run Automated Validation`` from the ``Runtime Validation Setup``
     menu.
  4. Select ``Build``.

The complete test suite takes around 65 minutes to complete. See
:ref:`validation_cam_tests` for more details.

The following messages are expected in the output to validate this Use-Case:

.. code-block:: text

   RESULTS - test_40_cam.CAMServiceTest.test_cam_service_boot_on_si: PASSED (0.00s)
   RESULTS - test_40_cam.CAMTest.test_cam_app_example_to_service_on_si: PASSED (63.55s)
   RESULTS - test_40_cam.CAMTest.test_cam_app_example_to_service_on_si_with_multiple_connections: PASSED (62.12s)
   RESULTS - test_40_cam.CAMTest.test_cam_ptp_sync: PASSED (143.39s)
   RESULTS - test_40_cam.CAMTest.test_cam_tool_deploy_to_si: PASSED (63.58s)
   RESULTS - test_40_cam.CAMTest.test_cam_tool_pack: PASSED (62.96s)
   RESULTS - test_40_cam.CAMTest.test_data_calibration: PASSED (175.01s)
   RESULTS - test_40_cam.CAMTest.test_logical_check_on_si: PASSED (26.43s)
   RESULTS - test_40_cam.CAMTest.test_temporal_check_on_si: PASSED (78.28s)
   RESULTS - test_40_cam.CAMTestDomU2.test_cam_app_example_to_service_on_si: PASSED (60.84s)
   RESULTS - test_40_cam.CAMTestDomU2.test_cam_app_example_to_service_on_si_with_multiple_connections: PASSED (63.07s)
   RESULTS - test_40_cam.CAMTestDomU2.test_cam_ptp_sync: PASSED (13.27s)
   RESULTS - test_40_cam.CAMTestDomU2.test_cam_tool_deploy_to_si: PASSED (32.96s)
   RESULTS - test_40_cam.CAMTestDomU2.test_cam_tool_pack: PASSED (32.19s)
   RESULTS - test_40_cam.CAMTestDomU2.test_data_calibration: PASSED (100.73s)
   RESULTS - test_40_cam.CAMTestDomU2.test_logical_check_on_si: PASSED (25.67s)
   RESULTS - test_40_cam.CAMTestDomU2.test_temporal_check_on_si: PASSED (77.41s)
   RESULTS - test_40_cam.CAMTestMultiDom.test_cam_app_example_to_service_on_si_with_multiple_vms: PASSED (68.86s)

.. note::
    There is a rare known failure where a timeout might occur during test execution. 
    
    Refer to :ref:`releasenotes_knownissues` for possible workarounds.

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

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To build a Baremetal Architecture image:

1. Select ``Safety Island Actuation Demo`` from the ``Use-Case`` menu.
2. Select ``Baremetal`` from the ``Reference Software Stack Architecture`` menu.
3. Select ``Build``.

Run the FVP
^^^^^^^^^^^

To start the FVP and connect to the Primary Compute terminal (running Linux):

.. code-block:: text

  kas shell -c "../layers/meta-arm/scripts/runfvp -t tmux --verbose"

The user should wait for the system to boot and for the Linux prompt to appear.
The following image shows an example on how the terminal should look after the
fvp invocation.

  .. image:: ../images/actuation_fvp_run.*
   :align: center
   :alt: Arm Auto Solutions Linux Login Prompt

|

The Safety Island (SI) Cluster 2 terminal running the Actuation Service is
available via the tmux window titled ``terminal_uart_si_cluster2``. For ease of
navigation, it's recommended to join the SI Cluster 2 terminal window to the
Primary Compute terminal window and creating a tmux pane attached to the build
host machine in order to issue commands on it.

Follow the steps below to achieve the same:

1. Ensure that the tmux window titled ``terminal_ns_uart0`` is selected. If not,
   press ``Ctrl-b w`` from the tmux session, navigate to the tmux window titled
   ``terminal_ns_uart0`` using the arrow keys, then press the ``Enter`` key.
2. Press ``Ctrl-b %`` to add a new tmux pane which will be used to issue
   commands on the build host machine.
3. Press ``Ctrl-b :`` and then type ``join-pane -s :terminal_uart_si_cluster2``
   followed by pressing ``Enter`` key to join the Actuation Service terminal
   window to the Primary Compute terminal window.

Refer to the following image of the tmux panes rearrangement. Panes can
be navigated using ``Ctrl-b`` followed by the arrow keys.

  .. image:: ../images/actuation_fvp_rearrange_windows.*
    :align: center
    :alt: Arm Auto Solutions FVP Rearrange Windows

|

The Reference Software Stack running on the Primary Compute can be logged into
as ``root`` user without a password in the Linux terminal. Run the below
command to guarantee that all the expected services have been
initialized.

.. code-block:: text

  systemctl is-system-running --wait

Wait for it to return. The expected terminal output is ``running``.

Run the Demo
^^^^^^^^^^^^

1. Run the ``ping`` command from the Primary Compute terminal (running Linux)
   to verify that it can communicate with the Safety Island Cluster 2 
   (running Zephyr):

   .. code-block:: text

      ping 192.168.2.1 -c 10

   The output should look like the following line, repeated 10 times:

   .. code-block:: text

      64 bytes from 192.168.2.1 seq=0 ttl=64 time=0.151 ms

2. From the tmux pane started for the build host machine terminal, start the
   Packet Analyzer:

   .. code-block:: text

      cd ~/arm-auto-solutions/
      kas shell -c "oe-run-native packet-analyzer-native start_analyzer -L debug -a localhost -c ./data"

   The following messages are expected from the host terminal:

   .. code-block:: text

      INFO : analyzer_client.py/_connect_to: Starting analyze, use Ctrl-C to stop the process.
      INFO : analyzer_client.py/_connect_to: Attempting a connect to (localhost : 49152)
      INFO : analyzer_client.py/_connect_to: Successfully connected to (localhost : 49152)

   A message similar to the following should appear on the SI Cluster 2
   terminal:

   .. code-block:: text

      Actuation Service initialized.
      Accepted tcp connection from the Packet Analyzer: <11>

   Refer to the following image for an invocation example of the Packet
   Analyzer.

     .. image:: ../images/actuation_packet_analyzer_baremetal.*
       :align: center
       :alt: Arm Auto Solutions Packet Analyzer - Baremetal Architecture

|

3. Start the Player on the Primary Compute terminal which replays a recording
   of a driving scenario:

   .. code-block:: text

      actuation_player -p /usr/share/actuation_player/

   A message similar to the following should appear on the Primary Compute
   terminal:

   .. code-block:: text

      Waiting for readers...     │1146571223376: -0.0000 (m/s^2) | 0.0000 (rad)
      ...Done. Starting replay.  │Thread get_analyzer_handle performing a blocking accept

   A message similar to the following should appear on the SI Cluster 2
   terminal:

   .. code-block:: text

    51572682601: -0.0000 (m/s^2) |  0.0000 (rad)
    51597466928: -0.0000 (m/s^2) |  0.0000 (rad)
    51622532911: -0.0000 (m/s^2) |  0.0000 (rad)
    51647642316: -0.0000 (m/s^2) |  0.0000 (rad)
    51672535849: -0.0000 (m/s^2) |  0.0000 (rad)
    51697376579: -0.0000 (m/s^2) |  0.0000 (rad)
    51722500414: -0.0000 (m/s^2) |  0.0000 (rad)
    51747622543: -0.0000 (m/s^2) |  0.0000 (rad)
    51772496466: -0.0000 (m/s^2) |  0.0000 (rad)
    Thread get_analyzer_handle performing a blocking accept

   A message similar to the following should appear on the host terminal where
   the Packet Analyzer is running:

   .. code-block:: text

    INFO : analyzer_client.py/_connect_to: Starting analyze, use Ctrl-C to stop the process.
    INFO : analyzer_client.py/_connect_to: Attempting a connect to (localhost : 49152)
    INFO : analyzer_client.py/_connect_to: Successfully connected to (localhost : 49152)
    INFO : analyzer_client.py/run_analyze_on_chain: (1) Analyzer synced with packet chain

   The following messages should appear on the host terminal, but values may
   differ once the Packet Analyzer has finished running:

   .. code-block:: text

    INFO : analyzer_client.py/run_analyze_on_chain: All expected control packets received
    INFO : analyzer_client.py/_log_jitter: Observed Frequency = 21.36147200, Avg Jitter = 0.02624593, Std Deviation:0.06096328
    INFO : analyzer_client.py/run_analyze_on_chain: End of cycle: AnalyzerResult.SUCCESS

    INFO : analyzer_client.py/_tear_conn: Received fin ack from Actuation Service

    Chain ID   Result
    0          AnalyzerResult.SUCCESS

4. To shut down the FVP and terminate the emulation automatically, follow the
   below steps:

    * Issue the following command on the Primary Compute terminal.

      .. code-block:: text

         shutdown now

      The below messages indicate the shutdown process is complete.

      .. code-block:: text

         [  OK  ] Finished System Power Off.
         [  OK  ] Reached target System Power Off.
         reboot: Power down

    * Close the tmux pane started for the build host machine by pressing
      ``Ctrl-d``.

Automated Validation
^^^^^^^^^^^^^^^^^^^^

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To run the validation tests:

  1. Select ``Safety Island Actuation Demo`` as ``Use-Case``.
  2. Select ``Baremetal`` from the ``Reference Software Stack Architecture``
     menu.
  3. Select ``Run Automated Validation`` from the ``Runtime Validation Setup``
     menu.
  4. Select ``Build``.

The complete test suite takes around 45 minutes to complete. See
:ref:`validation_actuation_demo` for more details.

The following messages are expected in the output to validate this Use-Case:

.. code-block:: text

  RESULTS - test_30_actuation.ActuationTest.test_analyzer_help: PASSED (0.44s)
  RESULTS - test_30_actuation.ActuationTest.test_ping: PASSED (26.04s)
  RESULTS - test_30_actuation.ActuationTest.test_player_to_analyzer: PASSED (167.15s)

.. note::
    There is a rare known failure where a timeout might occur during test execution. 
    
    Refer to :ref:`releasenotes_knownissues` for possible workarounds.

Virtualization Architecture
---------------------------

Build
^^^^^

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To build a Virtualization Architecture image:

1. Select ``Safety Island Actuation Demo`` from the ``Use-Case`` menu.
2. Select ``Virtualization`` from the ``Reference Software Stack Architecture``
   menu.
3. Select ``Build``.

Run the FVP
^^^^^^^^^^^

To start the FVP and connect to the Primary Compute terminal (running Linux):

.. code-block:: text

  kas shell -c "../layers/meta-arm/scripts/runfvp -t tmux --verbose"

The user should wait for the system to boot and for the Linux prompt to appear.
On a Virtualization Architecture image, this will access the Dom0 terminal.
The following image shows an example on how the terminal should look after the
fvp invocation.

  .. image:: ../images/actuation_fvp_run.*
   :align: center
   :alt: Arm Auto Solutions Linux Login Prompt

|

The Safety Island (SI) Cluster 2 terminal running the Actuation Service is
available via the tmux window titled ``terminal_uart_si_cluster2``. For ease of
navigation, it's recommended to join the SI Cluster 2 terminal window to the
Primary Compute terminal window and creating a tmux pane attached to the build
host machine in order to issue commands on it.

Follow the steps below to achieve the same:

1. Ensure that the tmux window titled ``terminal_ns_uart0`` is selected. If not,
   press ``Ctrl-b w`` from the tmux session, navigate to the tmux window titled
   ``terminal_ns_uart0`` using the arrow keys, then press the ``Enter`` key.
2. Press ``Ctrl-b %`` to add a new tmux pane which will be used to issue
   commands on the build host machine.
3. Press ``Ctrl-b :`` and then type ``join-pane -s :terminal_uart_si_cluster2``
   followed by pressing ``Enter`` key to join the Actuation Service terminal
   window to the Primary Compute terminal window.

Refer to the following image of the tmux panes rearrangement. Panes can
be navigated using ``Ctrl-b`` followed by the arrow keys.

  .. image:: ../images/actuation_fvp_rearrange_windows.*
    :align: center
    :alt: Arm Auto Solutions FVP Rearrange Windows

|

The Reference Software Stack running on the Primary Compute can be logged into
as ``root`` user without a password in the Linux terminal. Run the below
command to guarantee that all the expected services have been
initialized.

.. code-block:: text

  systemctl is-system-running --wait

Wait for it to return. The expected terminal output is ``running``.

.. note::
  A message similar to the following might appear in the Primary Compute
  terminal ``(XEN) d2v0: vGICR: SGI: unhandled word write 0x000000ffffffff
  to ICACTIVER0``, this is an expected behavior.

Run the Demo
^^^^^^^^^^^^

1. Enter the DomU1 console using the ``xl`` tool:

   .. code-block:: text

      xl console domu1

   DomU1 can be logged into as ``root`` user without a password in the Linux
   terminal. This command will provide a console on the DomU1. To exit,
   enter ``Ctrl-]`` (to access the FVP telnet shell), followed by typing
   ``send esc`` into the telnet shell and pressing ``Enter``. See the
   `xl documentation`_ for further details.

2. Run the ``ping`` command from the DomU1 terminal (running Linux)
   to verify that it can communicate with the Safety Island Cluster 2
   (running Zephyr):

   .. code-block:: text

      ping 192.168.2.1 -c 10

   The output should look like the following line, repeated 10 times:

   .. code-block:: text

      64 bytes from 192.168.2.1 seq=0 ttl=64 time=0.151 ms

3. From the tmux pane started for the build host machine terminal, start the
   Packet Analyzer:

   .. code-block:: text

      cd ~/arm-auto-solutions/
      kas shell -c "oe-run-native packet-analyzer-native start_analyzer -L debug -a localhost -c ./data"

   The following messages are expected from the host terminal:

   .. code-block:: text

      INFO : analyzer_client.py/_connect_to: Starting analyze, use Ctrl-C to stop the process.
      INFO : analyzer_client.py/_connect_to: Attempting a connect to (localhost : 49152)
      INFO : analyzer_client.py/_connect_to: Successfully connected to (localhost : 49152)

   A message similar to the following should appear on the SI Cluster 2
   terminal:

   .. code-block:: text

      Actuation Service initialized.
      Accepted tcp connection from the Packet Analyzer: <11>

   Refer to the following image for an invocation example of the Packet
   Analyzer.

     .. image:: ../images/actuation_packet_analyzer_virtualization.*
       :align: center
       :alt: Arm Auto Solutions Packet Analyzer - Virtualization Architecture

|

4. Start the Player on DomU1 which replays a recording of a driving scenario:

   .. code-block:: text

      actuation_player -p /usr/share/actuation_player/

   A message similar to the following should appear on the Primary Compute
   terminal:

   .. code-block::text

      Waiting for readers...
      ...Done. Starting replay.

   A message similar to the following should appear on the SI Cluster 2
   terminal:

   .. code-block:: text

    51572682601: -0.0000 (m/s^2) |  0.0000 (rad)
    51597466928: -0.0000 (m/s^2) |  0.0000 (rad)
    51622532911: -0.0000 (m/s^2) |  0.0000 (rad)
    51647642316: -0.0000 (m/s^2) |  0.0000 (rad)
    51672535849: -0.0000 (m/s^2) |  0.0000 (rad)
    51697376579: -0.0000 (m/s^2) |  0.0000 (rad)
    51722500414: -0.0000 (m/s^2) |  0.0000 (rad)
    51747622543: -0.0000 (m/s^2) |  0.0000 (rad)
    51772496466: -0.0000 (m/s^2) |  0.0000 (rad)
    Thread get_analyzer_handle performing a blocking accept

   A message similar to the following should appear on the host terminal where
   the Packet Analyzer is running:

   .. code-block:: text

    INFO : analyzer_client.py/_connect_to: Starting analyze, use Ctrl-C to stop the process.
    INFO : analyzer_client.py/_connect_to: Attempting a connect to (localhost : 49152)
    INFO : analyzer_client.py/_connect_to: Successfully connected to (localhost : 49152)
    INFO : analyzer_client.py/run_analyze_on_chain: (1) Analyzer synced with packet chain

   The following messages should appear, but values may differ once the Packet
   Analyzer has finished running:

   .. code-block:: text

    INFO : analyzer_client.py/run_analyze_on_chain: All expected control packets received
    INFO : analyzer_client.py/_log_jitter: Observed Frequency = 21.36147200, Avg Jitter = 0.02624593, Std Deviation:0.06096328
    INFO : analyzer_client.py/run_analyze_on_chain: End of cycle: AnalyzerResult.SUCCESS

    INFO : analyzer_client.py/_tear_conn: Received fin ack from Actuation Service

    Chain ID   Result
    0          AnalyzerResult.SUCCESS

5. To leave the DomU1 console, type ``Ctrl-]`` and enter ``send esc``.

6. To shut down the FVP and terminate the emulation automatically, follow the
   below steps:

    * Issue the following command on the Primary Compute terminal.

      .. code-block:: text

         shutdown now

      The below messages indicate the shutdown process is complete.

      .. code-block:: text

         [  OK  ] Finished System Power Off.
         [  OK  ] Reached target System Power Off.
         reboot: Power down

    * Close the tmux pane started for the build host machine by pressing
      ``Ctrl-d``.

Automated Validation
^^^^^^^^^^^^^^^^^^^^

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To run the validation tests:

  1. Select ``Safety Island Actuation Demo`` as ``Use-Case``.
  2. Select ``Virtualization`` from the
     ``Reference Software Stack Architecture`` menu.
  3. Select ``Run Automated Validation`` from the ``Runtime Validation Setup``
     menu.
  4. Select ``Build``.

The complete test suite takes around 110 minutes to complete. See
:ref:`validation_actuation_demo` for more details.

The following messages are expected in the output to validate this Use-Case:

.. code-block:: text

  RESULTS - test_30_actuation.ActuationTest.test_analyzer_help: PASSED (0.40s)
  RESULTS - test_30_actuation.ActuationTest.test_ping: PASSED (42.15s)
  RESULTS - test_30_actuation.ActuationTest.test_player_to_analyzer: PASSED (265.77s)

.. note::
    There is a rare known failure where a timeout might occur during test execution. 
    
    Refer to :ref:`releasenotes_knownissues` for possible workarounds.

.. _user_guide_reproduce_hipc:

Safety Island Communication Demo (using HIPC)
=============================================

The Safety Island Communication Demo uses :ref:`HIPC (Heterogeneous
Inter-processor Communication) <design/hipc:Heterogeneous Inter-processor
Communication (HIPC)>` to validate networking between the Primary Compute and
the three Safety Island clusters.

Baremetal Architecture
----------------------

Build
^^^^^

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To build a Baremetal Architecture image:

1. Select ``Safety Island Communication Demo (using HIPC)`` from the
   ``Use-Case`` menu.
2. Select ``Baremetal`` from the ``Reference Software Stack Architecture``
   menu.
3. Select ``Build``.

Automated Validation
^^^^^^^^^^^^^^^^^^^^

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To run the validation tests:

  1. Select ``Safety Island Communication Demo (using HIPC)`` as ``Use-Case``.
  2. Select ``Baremetal`` from the ``Reference Software Stack Architecture``
     menu.
  3. Select ``Run Automated Validation`` from the ``Runtime Validation Setup``
     menu.
  4. Select ``Build``.

The complete test suite takes around 45 minutes to complete. See
:ref:`validation_hipc_demo` for more details.

The following messages are expected in the output to validate this Use-Case:

.. code-block:: text

  RESULTS - test_30_hipc.HIPCTestBase.test_hipc_cluster0: PASSED (223.04s)
  RESULTS - test_30_hipc.HIPCTestBase.test_hipc_cluster1: PASSED (351.62s)
  RESULTS - test_30_hipc.HIPCTestBase.test_hipc_cluster2: PASSED (426.17s)
  RESULTS - test_30_hipc.HIPCTestBase.test_hipc_cluster_cl0_cl1: PASSED (100.46s)
  RESULTS - test_30_hipc.HIPCTestBase.test_hipc_cluster_cl0_cl2: PASSED (120.00s)
  RESULTS - test_30_hipc.HIPCTestBase.test_hipc_cluster_cl1_cl2: PASSED (161.80s)
  RESULTS - test_30_hipc.HIPCTestBase.test_ping_cl0_cl1: PASSED (37.34s)
  RESULTS - test_30_hipc.HIPCTestBase.test_ping_cl0_cl2: PASSED (36.80s)
  RESULTS - test_30_hipc.HIPCTestBase.test_ping_cl1_cl2: PASSED (37.10s)
  RESULTS - test_30_hipc.HIPCTestBase.test_ping_cluster0: PASSED (102.31s)
  RESULTS - test_30_hipc.HIPCTestBase.test_ping_cluster1: PASSED (95.50s)
  RESULTS - test_30_hipc.HIPCTestBase.test_ping_cluster2: PASSED (92.91s)
  RESULTS - test_30_ptp.PTPTest.test_ptp_linux_services: PASSED (4.56s)
  RESULTS - test_30_ptp.PTPTest.test_ptp_si_clients: PASSED (34.73s)

.. note::
    There is a rare known failure where a timeout might occur during test execution. 
    
    Refer to :ref:`releasenotes_knownissues` for possible workarounds.

Virtualization Architecture
---------------------------

Build
^^^^^

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To build a Virtualization Architecture image:

1. Select ``Safety Island Communication Demo (using HIPC)`` as ``Use-Case``.
2. Select ``Virtualization`` from the ``Reference Software Stack Architecture``
   menu.
3. Select ``Build``.


Automated Validation
^^^^^^^^^^^^^^^^^^^^

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To run the validation tests:

  1. Select ``Safety Island Communication Demo (using HIPC)`` as ``Use-Case``.
  2. Select ``Virtualization`` from the
     ``Reference Software Stack Architecture`` menu.
  3. Select ``Run Automated Validation`` from the ``Runtime Validation Setup``
     menu.
  4. Select ``Build``.

The complete test suite takes around 90 minutes to complete. See
:ref:`validation_hipc_demo` for more details.

The following messages are expected in the output to validate this Use-Case:

.. code-block:: text

  RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_hipc_cluster0: PASSED (311.77s)
  RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_hipc_cluster1: PASSED (389.55s)
  RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_hipc_cluster2: PASSED (413.83s)
  RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_hipc_cluster_cl0_cl1: PASSED (114.97s)
  RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_hipc_cluster_cl0_cl2: PASSED (138.32s)
  RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_hipc_cluster_cl1_cl2: PASSED (182.33s)
  RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_ping_cl0_cl1: PASSED (60.64s)
  RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_ping_cl0_cl2: PASSED (61.63s)
  RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_ping_cl1_cl2: PASSED (61.06s)
  RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_ping_cluster0: PASSED (153.76s)
  RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_ping_cluster1: PASSED (154.39s)
  RESULTS - test_30_hipc_virtualization.HIPCTestDomU1.test_ping_cluster2: PASSED (153.34s)
  RESULTS - test_30_hipc_virtualization.HIPCTestDomU2.test_hipc_cluster1: PASSED (365.17s)
  RESULTS - test_30_hipc_virtualization.HIPCTestDomU2.test_ping_cluster1: PASSED (152.43s)
  RESULTS - test_30_ptp_virtualization.PTPTestDom0.test_ptp_linux_services: PASSED (7.17s)
  RESULTS - test_30_ptp_virtualization.PTPTestDom0.test_ptp_si_clients: PASSED (50.57s)
  RESULTS - test_30_ptp_virtualization.PTPTestDomU1.test_ptp_domu_client: PASSED (47.37s)
  RESULTS - test_30_ptp_virtualization.PTPTestDomU1.test_ptp_linux_services: PASSED (7.17s)
  RESULTS - test_30_ptp_virtualization.PTPTestDomU2.test_ptp_domu_client: PASSED (50.57s)
  RESULTS - test_30_ptp_virtualization.PTPTestDomU2.test_ptp_linux_services: PASSED (47.37s)

.. note::
    There is a rare known failure where a timeout might occur during test execution. 
    
    Refer to :ref:`releasenotes_knownissues` for possible workarounds.

.. _user_guide_reproduce_parsec_tls:

Parsec-enabled TLS Demo
=======================

The demo can be run on the Baremetal Architecture. It consists of a TLS server
and a TLS client application. 

Refer to :ref:`design_applications_parsec_enabled_tls` for more information on
this application. This demo is included as part of the 
``Safety Island Actuation Demo``.

Baremetal Architecture
----------------------

Build
^^^^^

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To build a Baremetal Architecture image:

1. Select ``Safety Island Actuation Demo`` from the ``Use-Case`` menu.
2. Select ``Baremetal`` from the ``Reference Software Stack Architecture`` menu.
3. Select ``Build``.

Run the FVP
^^^^^^^^^^^

To start the FVP and connect to the Primary Compute terminal (running Linux):

.. code-block:: text

  kas shell -c "../layers/meta-arm/scripts/runfvp -t tmux --verbose"

The user should wait for the system to boot and for the Linux prompt to appear.

The Reference Software Stack running on the Primary Compute can be logged into
as ``root`` user without a password in the Linux terminal. Run the below
command to guarantee that all the expected services have been
initialized.

.. code-block:: text

  systemctl is-system-running --wait

Wait for it to return. The expected terminal output is ``running``.

Run the Demo
^^^^^^^^^^^^

The demo consists of a TLS server and a TLS client application. 

Refer to :ref:`design_applications_parsec_enabled_tls` for more information on
this application.

1. Run ``ssl_server`` from the Primary Compute terminal in the background and
   press the ``Enter`` key to continue:

   .. code-block:: text

      ssl_server &

   A message similar to the following should appear:

   .. code-block:: text

        . Seeding the random number generator... ok
        . Loading the server cert. and key... ok
        . Bind on https://localhost:4433/ ... ok
        . Setting up the SSL data.... ok
        . Waiting for a remote connection ...

   The TLS client application can take an optional parameter as the TLS server
   IP address.
   The default value of the parameter is ``localhost``.

2. Run ``ssl_client1`` from the Primary Compute terminal in a container:

   .. code-block:: text

      docker run  --rm -v /run/parsec/parsec.sock:/run/parsec/parsec.sock -v /usr/bin/ssl_client1:/usr/bin/ssl_client1 --network host docker.io/library/ubuntu:22.04 ssl_client1

   After a few seconds, a message similar to the following should appear:

   .. code-block:: text

      Trying to pull docker.io/library/ubuntu:22.04...
      Getting image source signatures
      Copying blob a186900671ab done   | 
      Copying config 981912c48e done   | 
      Writing manifest to image destination

         . Seeding the random number generator... ok
         . Loading the CA root certificate ... ok (0 skipped)
         . Connecting to tcp/localhost/4433... ok
         . Performing the SSL/TLS handshake... ok
         . Setting up the SSL/TLS structure... ok
         . Performing the SSL/TLS handshake... ok
         < Read from client: 18 bytes read

      GET / HTTP/1.0

         > Write to client: 156 bytes written

      HTTP/1.0 200 OK
      Content-Type: text/html

      <h2>Mbed TLS Test Server</h2>
      <p>Successful connection using: TLS-ECDHE-RSA-WITH-CHACHA20-POLY1305-SHA256</p> 
      
       ok
        . Verifying peer X.509 certificate... ok
        > Write to server:  . Closing the connection... ok
       18 bytes written

      GET / HTTP/1.0

        < Read from server:  . Waiting for a remote connection ... 156 bytes read

      HTTP/1.0 200 OK
      Content-Type: text/html

      <h2>Mbed TLS Test Server</h2>
      <p>Successful connection using: TLS-ECDHE-RSA-WITH-CHACHA20-POLY1305-SHA256</p>

3. Stop the TLS server and synchronize the container image to the
   persistent storage:

     .. code-block:: text

        pkill ssl_server
        sync

4. To shut down the FVP and terminate the emulation automatically, issue the
   following command on the Primary Compute terminal.

   .. code-block:: text

      shutdown now

   The below messages indicate the shutdown process is complete.

   .. code-block:: text

      [  OK  ] Finished System Power Off.
      [  OK  ] Reached target System Power Off.
      reboot: Power down

Automated Validation
^^^^^^^^^^^^^^^^^^^^

For more details about the validation of Parsec demo, refer to
:ref:`validation_parsec_enabled_tls_demo`.

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To run the validation tests:

  1. Select ``Safety Island Actuation Demo`` as ``Use-Case``.
  2. Select ``Baremetal`` from the ``Reference Software Stack Architecture``
     menu.
  3. Select ``Run Automated Validation`` from the ``Runtime Validation Setup``
     menu.
  4. Select ``Build``.

The complete test suite takes around 45 minutes to complete. See
:ref:`validation_parsec_enabled_tls_demo` for more details.

The following messages are expected in the output to validate this Use-Case:

.. code-block:: text

  RESULTS - test_40_parsec.ParsecTest.test_parsec_demo: PASSED (479.25s)

.. note::
    There is a rare known failure where a timeout might occur during test execution. 
    
    Refer to :ref:`releasenotes_knownissues` for possible workarounds.

.. _user_guide_reproduce_pc_psa_ps_crypto_api_test:

Primary Compute PSA Secure Storage and Crypto APIs Architecture Test Suite
==========================================================================

The demo can be run on the Baremetal Architecture. 

Refer to :ref:`design_primary_compute_secure_services` for more information on
this application. This demo is included as part of the ``Critical Application
Monitoring Demo``.

Baremetal Architecture
----------------------

Build
^^^^^

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To build a Baremetal Architecture image:

1. Select ``Critical Application Monitoring Demo`` from the ``Use-Case`` menu.
2. Select ``Baremetal`` from the ``Reference Stack Architecture`` menu.
3. Select ``Build``.

Run the FVP
^^^^^^^^^^^

To start the FVP and connect to the Primary Compute terminal (running Linux):

.. code-block:: text

  kas shell -c "../layers/meta-arm/scripts/runfvp -t tmux --verbose"

The user should wait for the system to boot and for the Linux prompt to appear.

The Reference Software Stack running on the Primary Compute can be logged into
as ``root`` user without a password in the Linux terminal. Run the below
command to guarantee that all the expected services have been
initialized.

.. code-block:: text

  systemctl is-system-running --wait

Wait for it to return. The expected terminal output is ``running``.

Run the Demo
^^^^^^^^^^^^

The demo consists of simple tests run from the Linux terminal. 

Refer to :ref:`design_primary_compute_secure_services` for more information on
this application.

1. Run the PSA Crypto API tests from the Primary Compute terminal using the
   following command:

   .. code-block:: text

      psa-crypto-api-test

   A message similar to the following should appear once the tests have
   completed:

   .. code-block:: text

      ************ Crypto Suite Report **********
      TOTAL TESTS     : 59
      TOTAL PASSED    : 59
      TOTAL SIM ERROR : 0
      TOTAL FAILED    : 0
      TOTAL SKIPPED   : 0
      ******************************************


2. Run the PSA Protected Storage API tests from the Primary Compute terminal
   using the following command:

   .. code-block:: text

      psa-ps-api-test

   A message similar to the following should appear once the tests have
   completed:

   .. code-block:: text

      ************ Storage Suite Report **********
      TOTAL TESTS     : 17
      TOTAL PASSED    : 11
      TOTAL SIM ERROR : 0
      TOTAL FAILED    : 0
      TOTAL SKIPPED   : 6
      ******************************************

3. Run the PSA Internal Trusted Storage API tests from the Primary Compute terminal
   using the following command:

   .. code-block:: text

      psa-its-api-test

   A message similar to the following should appear once the tests have
   completed:

   .. code-block:: text

      ************ Storage Suite Report **********
      TOTAL TESTS     : 10
      TOTAL PASSED    : 10
      TOTAL SIM ERROR : 0
      TOTAL FAILED    : 0
      TOTAL SKIPPED   : 0
      ******************************************

4. To shut down the FVP and terminate the emulation automatically, issue the
   following command on the Primary Compute terminal.

   .. code-block:: text

      shutdown now

   The below messages indicate the shutdown process is complete.

   .. code-block:: text

      [  OK  ] Finished System Power Off.
      [  OK  ] Reached target System Power Off.
      reboot: Power down

Automated Validation
^^^^^^^^^^^^^^^^^^^^

For more details about the validation of PSA Architecture Test Suite, refer to
:ref:`validation_trusted_services_tests`.

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To run the validation tests:

  1. Select ``Critical Application Monitoring Demo`` as ``Use-Case``.
  2. Select ``Baremetal`` from the ``Reference Stack Architecture`` menu.
  3. Select ``Run Automated Validation`` from the ``Runtime Validation Setup``
     menu.
  4. Select ``Build``.

The complete test suite takes around 40 minutes to complete. See
:ref:`validation_trusted_services_tests` for more details.

The following messages are expected in the output to validate this Use-Case:

.. code-block:: text

   RESULTS - test_50_trusted_services.ArmAutoSolutionsTrustedServices.test_03_psa_crypto_api_test: PASSED (298.70s)
   RESULTS - test_50_trusted_services.ArmAutoSolutionsTrustedServices.test_04_psa_its_api_test: PASSED (17.47s)
   RESULTS - test_50_trusted_services.ArmAutoSolutionsTrustedServices.test_05_psa_ps_api_test: PASSED (68.88s)

.. note::
    There is a rare known failure where a timeout might occur during test execution. 
    
    Refer to :ref:`releasenotes_knownissues` for possible workarounds.

.. _user_guide_reproduce_si_psa_ps_api_test:

Safety Island PSA Secure Storage APIs Architecture Test Suite
=============================================================

The demo can be run on the Baremetal Architecture.
See :ref:`design_applications_psa_arch_tests` for further details.

Baremetal Architecture
----------------------

Build
^^^^^

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To build a Baremetal Architecture image:

1. Select ``Safety Island PSA Secure Storage APIs Architecture Test Suite``
   from the ``Use-Case`` menu.
2. Select ``Build``.

Run the FVP
^^^^^^^^^^^

To start the FVP:

.. code-block:: text

  kas shell -c "../layers/meta-arm/scripts/runfvp -t tmux --verbose"

The Safety Island Cluster 2 terminal running the ``PSA Secure Storage APIs
Architecture Test Suite`` is available via the tmux window titled
``terminal_uart_si_cluster2``.

The Safety Island Cluster 2 tmux window can be accessed by typing ``Ctrl-b w``,
using the arrow keys to select ``terminal_uart_si_cluster2`` then pressing the
``Enter`` key.

Run the Demo
^^^^^^^^^^^^

The tests will automatically run. A log similar to the following should be
visible; it is normal for some tests to be skipped but there should be no
failed tests:

.. code-block:: text

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

.. note::
    This use-case does not require waiting for the Primary Compute to boot.

To shut down the FVP and terminate the emulation, select the terminal titled as
``python3`` where the ``runfvp`` was launched by pressing ``Ctrl-b 0`` and press
``Ctrl-c`` to stop the FVP process.

Automated Validation
^^^^^^^^^^^^^^^^^^^^

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To run the validation tests:

  1. Select ``Safety Island PSA Secure Storage APIs Architecture Test Suite``
     from the ``Use-Case`` menu.
  2. Select ``Run Automated Validation`` from the ``Runtime Validation Setup``
     menu.
  3. Select ``Build``.

The complete test suite takes around 15 minutes to complete. See
:ref:`validation_si_psa_arch_tests` for more details.

The following message is expected in the output to validate this Use-Case:

.. code-block:: text

  RESULTS - test_10_si_psa_arch_tests.SIPSAArchTests.test_psa_si_cluster2: PASSED (0.00s)

.. note::
    There is a rare known failure where a timeout might occur during test execution. 
    
    Refer to :ref:`releasenotes_knownissues` for possible workarounds.

.. _user_guide_reproduce_si_psa_crypto_api_test:

Safety Island PSA Crypto APIs Architecture Test Suite
=====================================================

The demo can be run on the Baremetal Architecture.
See :ref:`design_applications_psa_arch_tests` for further details.

Baremetal Architecture
----------------------

Build
^^^^^

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To build a Baremetal Architecture image:

1. Select ``Safety Island PSA Crypto APIs Architecture Test Suite``
   from the ``Use-Case`` menu.
2. Select ``Build``.

Run the FVP
^^^^^^^^^^^

To start the FVP:

.. code-block:: text

  kas shell -c "../layers/meta-arm/scripts/runfvp -t tmux --verbose"

The ``PSA Crypto APIs Architecture Test Suite`` is deployed on all the 3 Safety
Island (SI) Clusters. The test results can be seen on the following tmux
windows:

  * ``terminal_uart_si_cluster0``
  * ``terminal_uart_si_cluster1``
  * ``terminal_uart_si_cluster2``

The user can navigate through the windows mentioned above by pressing
``Ctrl-b w`` and arrow keys followed by the ``Enter`` key.

Run the tests
^^^^^^^^^^^^^

The tests will automatically run after the FVP is started. The complete test
suite takes around 5 minutes to complete.
When the tests finish, a log similar to the following should be visible.
Normally no failure should be seen::

  ************ Crypto Suite Report **********
  TOTAL TESTS     : 61
  TOTAL PASSED    : 61
  TOTAL SIM ERROR : 0
  TOTAL FAILED    : 0
  TOTAL SKIPPED   : 0
  ******************************************

.. note::
    This use-case does not require waiting for the Primary Compute to boot.

To shut down the FVP and terminate the emulation, select the terminal titled as
``python3`` where the ``runfvp`` was launched by pressing ``Ctrl-b 0`` and press
``Ctrl-c`` to stop the FVP process.

Automated Validation
^^^^^^^^^^^^^^^^^^^^

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To run the validation tests:

  1. Select ``Safety Island PSA Crypto APIs Architecture Test Suite`` from the
     ``Use-Case`` menu.
  2. Select ``Run Automated Validation`` from the ``Runtime Validation Setup``
     menu.
  3. Select ``Build``.

The complete test suite takes around 35 minutes to complete. See
:ref:`validation_si_psa_arch_tests` for more details.

The following messages are expected in the output to validate this Use-Case:

.. code-block:: text

   RESULTS - test_10_si_psa_arch_tests.SIPSAArchTests.test_psa_si_cluster0: PASSED (0.01s)
   RESULTS - test_10_si_psa_arch_tests.SIPSAArchTests.test_psa_si_cluster1: PASSED (0.01s)
   RESULTS - test_10_si_psa_arch_tests.SIPSAArchTests.test_psa_si_cluster2: PASSED (0.01s)

.. note::
    There is a rare known failure where a timeout might occur during test execution. 
    
    :ref:`releasenotes_knownissues` for possible workarounds.

.. _user_guide_reproduce_fault_management:

Fault Management Demo
=====================

The demo uses the Safety Island Cluster 1 console and it can be run on the
Baremetal Architecture of the Safety Island Actuation Demo. 

Refer to :ref:`design_applications_fault_mgmt` for further details.

Baremetal Architecture
----------------------

Build
^^^^^

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To build the Baremetal Architecture image:

1. Select ``Safety Island Actuation Demo`` from the ``Use-Case`` menu.
2. Select ``Baremetal`` from the ``Reference Software Stack Architecture`` menu.
3. Select ``Build``.

Run the FVP
^^^^^^^^^^^

To start the FVP:

.. code-block:: text

  kas shell -c "../layers/meta-arm/scripts/runfvp -t tmux --verbose"

The Fault Management subsystem is deployed on Safety Island Cluster 1 so the
instructions below should be executed on its terminal.

The Safety Island Cluster 1 tmux window can be accessed by typing ``Ctrl-b w``,
using the arrow keys to select ``terminal_uart_si_cluster1`` then pressing the
``Enter`` key.

Run the Demo
^^^^^^^^^^^^

The instructions below demonstrate injecting faults into both the System FMU
and GIC-720AE FMU and how this affects the SSU safety state.

 1. Start by enumerating the configured fault device tree:

    .. code-block:: text

      fault tree

    The output shows the root fault device ``fmu@2a510000`` (the System FMU),
    after which are the attached safety state device ``ssu@2a500000``, the
    fault device ``fmu@2a570000`` (the Primary Compute GIC-720AE FMU) and the
    fault device ``fmu@2a530000`` (the Safety Island GIC-720AE FMU):

    .. code-block:: text

      Root 0: fmu@2a510000
              Safety: ssu@2a500000
              Slot 0: fmu@2a570000
              Slot 1: fmu@2a530000

 2. After booting, query the initial state of the SSU:

    .. code-block:: text

      fault safety_status ssu@2a500000

    The initial state is TEST:

    .. code-block:: text

      Status: TEST (0x0)

 3. It is expected that a Fault Management deployment would perform a self-test
    after boot then signal its outcome to the SSU. For demonstration purposes,
    simulate a successful self-test completion by issuing the
    ``compl_ok`` signal to the SSU:

    .. code-block:: text

      fault safety_control ssu@2a500000 compl_ok

    The system is now ``SAFE`` for operation:

    .. code-block:: text

      Signal: compl_ok (0x0)
      State: SAFE (0x3)

 4. Simulate an internal *Lockstep error* (``0x4``) in the System FMU:

    .. code-block:: text

      fault inject fmu@2a510000 0x4

    Three events are logged:

     * The subsystem reports that it received the fault and that it was
       non-critical (all System FMU internal faults are non-critical).
     * The safety component reports that this caused the SSU to enter the
       ``ERRN`` state.
     * The storage component reports that the total historical fault count for
       this fault on this device is now ``1``.

    .. code-block:: text

      Injecting fault 0x4 to device fmu@2a510000
      [00:04:49.110,000] <inf> fault_mgmt: Fault received (non-critical): 0x4 on fmu@2a510000

      [00:04:49.110,000] <inf> fault_mgmt_safety: Safety status: ERRN (0x5) on ssu@2a500000

      [00:04:49.160,000] <inf> fault_mgmt_protected_storage: Fault count for 0x4 on fmu@2a510000: 1

 5. The SSU will remain in the ``ERRN`` state until signaled (unless a critical
    fault occurs). Send a ``compl_ok`` signal again to recover from this fault:

    .. code-block:: text

      fault safety_control ssu@2a500000 compl_ok

    The SSU is now in the ``SAFE`` state again:

    .. code-block:: text

      Signal: compl_ok (0x0)
      State: SAFE (0x3)

 6. Next, inject an *SPI collator external error* (``0x20000a00``) into the
    GIC-720AE FMU:

    .. code-block:: text

      fault inject fmu@2a570000 0x20000a00

    This results in a similar output to above, except that the received fault
    was critical and the safety status is now ``ERRC``. (GIC-720AE FMU faults
    are critical by default, but this can be changed from the shell using the
    ``fault set_critical`` sub-command).

    .. code-block:: text

      Injecting fault 0x20000a00 to device fmu@2a570000
      [00:09:13.210,000] <inf> fault_mgmt: Fault received (critical): 0x20000a00 on fmu@2a570000

      [00:09:13.210,000] <inf> fault_mgmt_safety: Safety status: ERRC (0x6) on ssu@2a500000

      [00:09:13.270,000] <inf> fault_mgmt_protected_storage: Fault count for 0x20000a00 on fmu@2a570000: 1

 7. The number of occurrences of each fault is tracked per device by the
    storage component. Inject another *Lockstep error* into the System FMU:

    .. code-block:: text

      fault inject fmu@2a510000 0x4

    The fault count is now ``2``. Note that the safety status is still
    ``ERRC``.

    .. code-block:: text

      Injecting fault 0x4 to device fmu@2a510000
      [00:14:02.800,000] <inf> fault_mgmt: Fault received (non-critical): 0x4 on fmu@2a510000

      [00:14:02.800,000] <inf> fault_mgmt_safety: Safety status:  ERRC (0x6) on ssu@2a500000

      [00:14:02.860,000] <inf> fault_mgmt_protected_storage: Fault count for 0x4 on fmu@2a510000: 2

    The full list of stored faults can also be queried:

    .. code-block:: text

      fault list

    This shows all the faults injected into both FMUs above:

    .. code-block:: text

      Fault history:
      Fault received (non-critical): 0x4 on fmu@2a510000 : count 2

      Fault received (critical): 0x20000a00 on fmu@2a570000 : count 1

 8. The ``ERRC`` represents a critical system failure and cannot be recovered
    by the software - confirm this by trying to issue ``compl_ok`` again:

    .. code-block:: text

      fault safety_control ssu@2a500000 compl_ok

    The SSU status is still ``ERRC``:

    .. code-block:: text

      Signal: compl_ok (0x0)
      State: ERRC (0x6)

    The state can now only be affected through a full system reset (e.g. by
    stopping and starting the FVP), after which the state will be ``TEST``
    once again.

 .. note::
    This use-case does not require waiting for the Primary Compute to boot.

 9. To shut down the FVP and terminate the emulation, select the terminal titled
    as ``python3`` where the ``runfvp`` was launched by pressing ``Ctrl-b 0``
    and press ``Ctrl-c`` to stop the FVP process.

See the :ref:`design_applications_fault_mgmt_shell_reference` for more details
about these and other Fault Management shell sub-commands.

Automated Validation
^^^^^^^^^^^^^^^^^^^^

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To run the validation tests:

  1. Select ``Safety Island Actuation Demo`` as ``Use-Case``.
  2. Select ``Baremetal`` from the ``Reference Software Stack Architecture``
     menu.
  3. Select ``Run Automated Validation`` from the ``Runtime Validation Setup``
     menu.
  4. Select ``Build``.

The complete test suite takes around 45 minutes to complete. See
:ref:`validation_fault_management` for more details.

The following messages are expected in the output to validate this Use-Case:

.. code-block:: text

  RESULTS - test_10_fault_mgmt.FaultMgmtSSUTest.test_gic_fmu_ssu_compl_ok: PASSED (96.24s)
  RESULTS - test_10_fault_mgmt.FaultMgmtSSUTest.test_ssu_ce_not_ok: PASSED (38.18s)
  RESULTS - test_10_fault_mgmt.FaultMgmtSSUTest.test_ssu_nce_ok: PASSED (29.24s)
  RESULTS - test_10_fault_mgmt.FaultMgmtTest.test_fmu_fault_clear: PASSED (11.24s)
  RESULTS - test_10_fault_mgmt.FaultMgmtTest.test_fmu_fault_count: PASSED (6.04s)
  RESULTS - test_10_fault_mgmt.FaultMgmtTest.test_fmu_fault_list: PASSED (36.22s)
  RESULTS - test_10_fault_mgmt.FaultMgmtTest.test_fmu_fault_summary: PASSED (22.64s)
  RESULTS - test_10_fault_mgmt.FaultMgmtTest.test_gic_fmu_inject: PASSED (18.69s)
  RESULTS - test_10_fault_mgmt.FaultMgmtTest.test_system_fmu_internal_inject: PASSED (8.77s)
  RESULTS - test_10_fault_mgmt.FaultMgmtTest.test_system_fmu_internal_set_enabled: PASSED (11.09s)
  RESULTS - test_10_fault_mgmt.FaultMgmtTest.test_tree: PASSED (0.71s)

.. note::
    There is a rare known failure where a timeout might occur during test execution. 

    Refer to :ref:`releasenotes_knownissues` for possible workarounds.

.. _user_guide_reproduce_arm_systemready_ir_validation:

Arm SystemReady IR Validation
=============================

Arm SystemReady IR Firmware Build
---------------------------------

The Arm SystemReady IR Firmware Build option just builds the
Arm SystemReady IR-aligned firmware. 

Refer to :ref:`design_systemready_ir` for more details.

.. image:: ../images/build_config_sr_ir.*
   :align: center
   :width: 60 %
   :alt: Arm Auto Solutions Build Configuration Menu - Arm SystemReady IR Firmware Build

|

Build
^^^^^

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To build the Arm SystemReady IR-aligned firmware image:

1. Select ``Arm SystemReady IR Firmware Build`` under
   ``Arm SystemReady IR Validation`` from the ``Use-Case`` menu.
2. Select ``Build``.

The firmware images listed below can be found in the directory
``build/tmp_systemready-glibc/deploy/images/fvp-rd-kronos/``.

 * ``ap-flash-image.img``
 * ``encrypted_cm_provisioning_bundle_0.bin``
 * ``encrypted_dm_provisioning_bundle.bin``
 * ``rse-flash-image.img``
 * ``rse-nvm-image.img``
 * ``rse-rom-image.img``

.. _user_guide_reproduce_sr_ir_acs:

Arm SystemReady IR Architecture Compliance Suite (ACS) Tests
------------------------------------------------------------

The ACS for the Arm SystemReady IR certification is delivered through a
live OS image, which enables the basic automation to run the tests.

The system will boot with the ACS live OS image and the ACS tests will run
automatically after the system boots. See :ref:`systemready_ir_acs_tests` for
more details.

Build and Automated Validation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To build and run the Arm SystemReady IR ACS tests:

1. Select ``Arm SystemReady IR Architecture Compliance Suite (ACS) Tests`` under
   ``Arm SystemReady IR Validation`` from the ``Use-Case`` menu.
2. Select ``Build``.

A similar output to the following is printed out:

.. code-block:: text

  NOTE: recipe arm-systemready-ir-acs-2.1.0-r0: task do_testimage: Started
  Creating terminal default on terminal_ns_uart0
  Creating terminal tf-a on terminal_sec_uart
  Creating terminal scp on terminal_uart_scp
  Creating terminal rse on terminal_rse_uart
  Creating terminal safety_island_c0 on terminal_uart_si_cluster0
  Creating terminal safety_island_c1 on terminal_uart_si_cluster1
  Creating terminal safety_island_c2 on terminal_uart_si_cluster2
  ACS SIE + bbr/bsa running
  Test Group (VariableAttributes): PASSED
  Test Group (VariableUpdates): FAILED
  Test Group (AuthVar_Conf): FAILED
  Test Group (AuthVar_Func): FAILED
  Test Group (BBSRVariableSizeTest_func): FAILED
  Test Group (ImageLoading): FAILED
  Test Group (PlatformSpecificElements): FAILED
  Test Group (RequiredElements): FAILED
  Test Group (CheckEvent_Conf): PASSED
  Test Group (CheckEvent_Func): PASSED
  Test Group (CloseEvent_Func): PASSED
  Test Group (CreateEventEx_Conf): PASSED
  Test Group (CreateEventEx_Func): PASSED
  Test Group (CreateEvent_Conf): PASSED
  Test Group (CreateEvent_Func): PASSED
  Test Group (RaiseTPL_Func): PASSED
  Test Group (RestoreTPL_Func): PASSED
  Test Group (SetTimer_Conf): PASSED
  Test Group (SetTimer_Func): PASSED
  Test Group (SignalEvent_Func): PASSED
  Test Group (WaitForEvent_Conf): PASSED
  Test Group (WaitForEvent_Func): PASSED
  Test Group (AllocatePages_Conf): PASSED
  Test Group (AllocatePages_Func): PASSED
  Test Group (AllocatePool_Conf): PASSED
  Test Group (AllocatePool_Func): PASSED
  Test Group (FreePages_Conf): PASSED
  Test Group (FreePages_Func): PASSED
  Test Group (GetMemoryMap_Conf): PASSED
  Test Group (GetMemoryMap_Func): PASSED
  ...
  ...
  Test Group (virtio_blk virtio1): vda
  Test Group (Supported ports):
  Linux tests complete
  RESULTS:
  RESULTS - arm_systemready_ir_acs.SystemReadyACSTest.test_acs: PASSED (32417.37s)
  SUMMARY:
  arm-systemready-ir-acs () - Ran 1 test in 32417.375s
  arm-systemready-ir-acs - OK - All required tests passed (successes=1, skipped=0, failures=0, errors=0)

As seen in the above logs, some Test Groups are expected to fail. The following
messages are expected to validate this Use-Case:

.. code-block:: text

  RESULTS - arm_systemready_ir_acs.SystemReadyACSTest.test_acs: PASSED (32417.37s)

.. note::

  Running the ACS tests more than once will have them resume from where they
  last stopped. Additionally, consecutive runs are not supported by the ACS
  logs; it will result in a failure after the end of the tests. Use the
  following to re-start the entire test suite properly:

  .. code-block:: text

    kas shell -c "bitbake arm-systemready-ir-acs -C unpack"

.. note::

  The ACS tests take hours to complete. The actual time taken will vary
  depending on the performance of the build host. The default timeout setting
  for the tests is 12 hours for an x86_64 host or 24 hours for an aarch64 host.
  If a timeout failure occurs, increase the timeout setting and re-run
  the tests with the following command on the build host terminal. The example
  command below changes the timeout setting to 16 hours.

  .. code-block:: text

     TEST_OVERALL_TIMEOUT="\${@16*60*60}" kas shell -c "bitbake arm-systemready-ir-acs -C unpack"

.. note::
    There is a rare known failure where a timeout might occur during test execution. 
    
    Refer to :ref:`releasenotes_knownissues` for possible workarounds.

Refer to :ref:`systemready_ir_acs_tests` for an explanation on how the
ACS tests are set up and how they work in the Reference Software Stack.

.. _user_guide_reproduce_arm_systemready_ir_linux:

Linux Distribution Installation (Debian, openSUSE and Fedora)
=============================================================

The Arm SystemReady IR-aligned firmware must boot at least three unmodified
generic UEFI distribution images from an ISO image.

This Software Stack currently supports three Linux distributions: `Debian Stable`_,
`openSUSE Leap`_ and `Fedora Server`_.

.. note::

  :ref:`systemready_ir_acs_tests` runs the SIE tests, which enrolls the
  authenticated variables for UEFI Secure Boot, so running the Linux distros
  installation after running the ACS tests will result in a failure.
  The firmware flash images need to be recreated with the following command:

  .. code-block:: text

    kas shell -c "bitbake firmware-fvp-rd-kronos -C deploy"

  Refer to :ref:`design_boot_process_uefi_secure_boot` for more information.

.. note::

  The manual installation of a Linux distribution requires some manual
  interaction, for example, some necessary selections or confirmations,
  entering the user and password, etc.

  The whole installation process takes a long time (possibly up to 10 hours, or
  even longer).

  We suggest that when running the Linux distribution installations the FVP is
  the only running process as it will consume large amounts of RAM that can make
  the system unstable.

Refer to :ref:`systemready_ir_linux_install` for an explanation on how
the Linux distros installation is set up and how they work in the Reference
Software Stack.

Debian
------

Distro Unattended Installation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Not supported in the current release.

Distro Manual Installation
^^^^^^^^^^^^^^^^^^^^^^^^^^

To install Debian, you can refer to the `Debian GNU/Linux Installation Guide`_.

Distro Installation Media Preparation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The firmware flash images need to be recreated with the following command:

.. code-block:: text

  kas shell -c "bitbake firmware-fvp-rd-kronos -C deploy"

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To build the Arm SystemReady IR Linux distros installation tests:

1. Select ``Debian Linux Distro Installation`` under
   ``Linux Distribution Installation (Debian, openSUSE and Fedora)`` from the
   ``Use-Case`` menu.
2. Select ``Build``.

.. image:: ../images/build_config_sr_distro_debian.*
   :align: center
   :width: 60 %
   :alt: Arm Auto Solutions Build Configuration Menu - Debian Linux Distro Installation

|

Distro Installation
~~~~~~~~~~~~~~~~~~~

Run the following command to start the installation:

.. code-block:: text

  kas shell -c "../layers/meta-arm/scripts/runfvp -t tmux --verbose"

The whole process of installing Debian will probably take about 5 hours. The
install process begins when you see the following:

    .. image:: ../images/sr-ir-linux-distro-debian-install-grub-4.*
       :align: center
       :width: 60 %
       :alt: Grub Install Options Menu - Debian Linux Distro Installation

Select ``Install`` to start the installation process.

The following is the problem that has been encountered during the Debian
installation process and how to solve it:

* Install the GRUB boot loader

  When the installation reaches the ``Install the GRUB boot loader`` phase,
  there will be an error ``Unable to install GRUB in dummy``.
  This is because on an EBBR platform, UEFI ``SetVariable()`` is not required at
  runtime (however, it is required at boot time).

  .. image:: ../images/sr-ir-linux-distro-debian-install-grub-0.*
     :align: center
     :width: 60 %
     :alt: Grub Installation Failure Prompt - Debian Linux Distro Installation

|

  One workaround we have is to "execute a shell" when the GRUB install phase
  throws the above error. To execute a shell, press ``Ctrl-a n`` to switch the
  debug shell, and run the following commands:

  .. code-block:: text

     chroot /target
     update-grub
     mkdir /boot/efi/EFI/BOOT
     cp -v /boot/efi/EFI/debian/grubaa64.efi /boot/efi/EFI/BOOT/bootaa64.efi

  A snapshot is as below:

  .. image:: ../images/sr-ir-linux-distro-debian-install-grub-1.*
     :align: center
     :width: 60 %
     :alt: Grub Workaround Console Output - Debian Linux Distro Installation

|

  After doing the above GRUB workaround, press ``Ctrl-a p`` to go back to the
  installer again. Select ``Continue`` on the GRUB failure screen.

  .. image:: ../images/sr-ir-linux-distro-debian-install-grub-2.*
     :align: center
     :width: 60 %
     :alt: Second Grub Installation Failure Prompt - Debian Linux Distro Installation

|

  Select ``Continue without boot loader`` in the ``Debian installer main menu``
  and continue.

  .. image:: ../images/sr-ir-linux-distro-debian-install-grub-3.*
     :align: center
     :width: 60 %
     :alt: Debian Installer Main Menu - Debian Linux Distro Installation

|

* Log in

  When the installation reaches the final ``Finishing the installation``
  phase, you will need to wait some time to finish the remaining tasks,
  and then it will automatically reboot into the installed OS. You can log into
  the Linux shell with the user created during installation.

* Terminate the FVP

  To shut down the FVP and terminate the emulation automatically, log into the
  Linux shell as the root user then run the following command.

      .. code-block:: text

         shutdown now

  The below message indicates the shutdown process is complete.

      .. code-block:: text

         reboot: Power down

  Subsequently running the FVP will boot into Debian.

openSUSE
--------

Distro Unattended Installation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In this test we have modified the installation ISO image to add the automatic
installation file inside it. This required adding the ``autoinst.xml`` file inside
the ISO image to locate the installation configuration file (:repo:`yocto/kas/\
patches/meta-arm-systemready/\
0001-arm-systemready-linux-distros-Implement-unattended-o.patch`)


Distro Installation
~~~~~~~~~~~~~~~~~~~

The firmware flash images need to be recreated with the following command:

.. code-block:: text

  kas shell -c "bitbake firmware-fvp-rd-kronos -C deploy"

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To build the Arm SystemReady IR Linux distros installation tests:

1. Select ``openSUSE Linux Distro Installation`` under
   ``Linux Distribution Installation (Debian, openSUSE and Fedora)`` from the
   ``Use-Case`` menu.
2. Select ``Build``.

.. image:: ../images/build_config_sr_distro_opensuse_unattended.*
   :align: center
   :width: 60 %
   :alt: Arm Auto Solutions Build Configuration Menu - openSUSE Linux Distro Installation

|

  A similar output to the following indicates when the installation is finished, which will take
  around 12 hours:

  .. code-block:: text

     Transitioned to on
     Installation status: Loading the kernel, initrd and basic drivers...
     Installation status: Starting hardware detection...
     Installation status: Loading Installation System...
     Installation status: Performing Installation...
     Installation status: Finishing Configuration...
     Installation status: openSUSE installation finished successfully.
     Transitioned to off
     RESULTS:
     RESULTS - arm_systemready_opensuse_unattended.SystemReadyOpenSUSEUnattendedTest.test_opensuse_unattended: PASSED (24367.99s)
     SUMMARY:
     arm-systemready-linux-distros-opensuse () - Ran 1 test in 24367.997s

* Log in

  After the installation is finished, run the following command to log
  into the Linux shell:

  .. code-block:: text

     kas shell -c "../layers/meta-arm/scripts/runfvp -t tmux --verbose"
  
  Log into the Linux shell with the user created during the installation
  using the username ``user`` and the password ``unsafe``.

* Terminate the FVP

  To shut down the FVP and terminate the emulation automatically, run the
  following command.

      .. code-block:: text

         sudo shutdown now

  The below message indicates the shutdown process is complete.

      .. code-block:: text

         reboot: Power down

  Subsequently running the FVP will boot into openSUSE.


Distro Manual Installation
^^^^^^^^^^^^^^^^^^^^^^^^^^

To install openSUSE, you can refer to the `openSUSE Installation Guide`_.

Distro Installation Media Preparation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The firmware flash images need to be recreated with the following command:

.. code-block:: text

  kas shell -c "bitbake firmware-fvp-rd-kronos -C deploy"

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To build the Arm SystemReady IR Linux distros installation tests:

1. Select ``openSUSE Linux Distro Installation`` under
   ``Linux Distribution Installation (Debian, openSUSE and Fedora)`` from the
   ``Use-Case`` menu.
2. Unselect ``Run Unattended Installation`` under
   ``Distros Unattended Installation Setup`` menu.
3. Select ``Build``.

.. image:: ../images/build_config_sr_distro_opensuse_manual.*
   :align: center
   :width: 60 %
   :alt: Arm Auto Solutions Build Configuration Menu - openSUSE Linux Distro Installation

|

Run the following command to start the installation:

.. code-block:: text

  kas shell -c "../layers/meta-arm/scripts/runfvp -t tmux --verbose"

The whole process of installing openSUSE will take several hours. The install
process begins when you see the following:

   .. image:: ../images/sr-ir-linux-distro-opensuse-install-installation.*
      :align: center
      :width: 60 %
      :alt: Leap Install Options Menu - openSUSE Linux Distro Installation

Select ``No`` when you get to the ``Online Repositories`` screen.

   .. image:: ../images/sr-ir-linux-distro-opensuse-install-online-repo.*
      :align: center
      :width: 60 %
      :alt: Online Repositories Options Menu - openSUSE Linux Distro Installation

Select ``Installation`` to start the installation process.

* System Role

  When you get to the ``System Role`` screen, select ``Server``, then select
  ``Next`` to continue with the installation.

   .. image:: ../images/sr-ir-linux-distro-opensuse-install-system-role.*
      :align: center
      :width: 60 %
      :alt: System Role Selection Menu - openSUSE Linux Distro Installation

   |

   .. tip::

      Use ``Tab`` to cycle through options on screens during installation.

* Installation process

  Once you have selected ``Install`` on the ``Confirm Installation`` screen, the
  installation will proceed and it will take several hours. The steps of the
  installation process are:

    * ``Installing Packages...``
    * ``Save configuration``
    * ``Save installation settings``
    * ``Install boot manager``
    * ``Prepare system for initial boot``
    * Then the system will reboot automatically in 10s, you can select ``OK`` to
      reboot immediately.

* Log in

  After the reboot process, log into the Linux shell with the user
  created during installation.

* Terminate the FVP

  To shut down the FVP and terminate the emulation automatically, run the
  following command.

  .. code-block:: text

     sudo shutdown now

  The below message indicates the shutdown process is complete.

      .. code-block:: text

         reboot: Power down

  Subsequently running the FVP will boot into openSUSE.

Fedora
------

Distro Unattended Installation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In this test we have modified the installation ISO image to add the kickstart
configuration file inside it. This required editing the ``grub.cfg`` file inside
the ISO image to locate the kickstart configuration file (:meta-arm-repo:`meta\
-arm-systemready/recipes-test/arm-systemready-linux-distros/unattended-boot\
-conf/Fedora/ks.cfg`).

Distro Installation
~~~~~~~~~~~~~~~~~~~

The firmware flash images need to be recreated with the following command:

.. code-block:: text

  kas shell -c "bitbake firmware-fvp-rd-kronos -C deploy"

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To build the Arm SystemReady IR Linux distros installation tests:

1. Select ``Fedora Linux Distro Installation`` under
   ``Linux Distribution Installation (Debian, openSUSE and Fedora)`` from the
   ``Use-Case`` menu.
2. Select ``Build``.

.. image:: ../images/build_config_sr_distro_fedora_unattended.*
   :align: center
   :width: 60 %
   :alt: Arm Auto Solutions Build Configuration Menu - Fedora Linux Distro Installation

|

  A similar output to the following indicates when the installation is finished, which will take
  around 12 hours:

  .. code-block:: text

     Transitioned to on
     Installation status: Loading the installer, kernel and initrd...
     Installation status: Setting up the installation environment...
     Installation status: Installing the software packages...
     Installation status: Fedora installation finished successfully.
     Transitioned to off
     RESULTS:
     RESULTS - arm_systemready_fedora_unattended.SystemReadyFedoraUnattendedTest.test_fedora_unattended: PASSED (38743.47s)
     SUMMARY:
     arm-systemready-linux-distros-fedora () - Ran 1 test in 38743.478s

* Log in

  After the installation is finished, run the following command to log
  into the Linux shell:

  .. code-block:: text

     kas shell -c "../layers/meta-arm/scripts/runfvp -t tmux --verbose"
  
  Log into the Linux shell with the user created during the installation
  using the username ``user`` and the password ``unsafe``.

* Terminate the FVP

  To shut down the FVP and terminate the emulation automatically, run the
  following command.

      .. code-block:: text

         sudo shutdown now

  The below message indicates the shutdown process is complete.

      .. code-block:: text

         reboot: Power down

  Subsequently running the FVP will boot into Fedora.

Distro Manual Installation
^^^^^^^^^^^^^^^^^^^^^^^^^^

To install Fedora, you can refer to the `Fedora Installation Guide`_.

Distro Installation Media Preparation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The firmware flash images need to be recreated with the following command:

.. code-block:: text

  kas shell -c "bitbake firmware-fvp-rd-kronos -C deploy"

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To build the Arm SystemReady IR Linux distros installation tests:

1. Select ``Fedora Linux Distro Installation`` under
   ``Linux Distribution Installation (Debian, openSUSE and Fedora)``
   from the ``Use-Case`` menu.
2. Unselect ``Run Unattended Installation`` under
   ``Distros Unattended Installation Setup`` menu.
3. Select ``Build``.

.. image:: ../images/build_config_sr_distro_fedora_manual.*
   :align: center
   :width: 60 %
   :alt: Arm Auto Solutions Build Configuration Menu - Fedora Linux Distro Installation

|

Distro Installation
~~~~~~~~~~~~~~~~~~~

Run the following command to start the installation:

.. code-block:: text

  kas shell -c "../layers/meta-arm/scripts/runfvp -t tmux --verbose"

The whole process of installing Fedora will probably take about 24 hours. The
install process begins when you see the following:

    .. image:: ../images/sr-ir-linux-distro-fedora-install-grub.*
       :align: center
       :width: 60 %
       :alt: Grub Install Options Menu - Fedora Linux Distro Installation

Select ``Install Fedora 39`` to start the installation process.

Here are some tips for installing Fedora:

1. It will take a few minutes for GRUB to load the installer, kernel and initrd.
2. Once the installer has started, enter ``2`` to choose ``Use text mode``.

   .. code-block:: console

      Starting installer, one moment...
      anaconda 39.32.6-2.fc39 for Fedora 39 started.
       * installation log files are stored in /tmp during the installation
       * shell is available on TTY2 and in second TMUX pane (ctrl+b, then press 2)
       * when reporting a bug add logs from /tmp as separate text/plain attachments

      X or window manager startup failed, falling back to text mode.
      ================================================================================
      ================================================================================
      X was unable to start on your machine. Would you like to start VNC to connect to
      this computer from another computer and perform a graphical installation or
      continue with a text mode installation?

      1) Start VNC
      2) Use text mode

      Please make a selection from the above ['c' to continue, 'h' to help, 'q' to
      quit, 'r' to refresh]: 2

3. When reaching the installation menu, you will see several items marked as ``!``
   which indicates that the item needs to be configured before proceeding.

   .. code-block:: console

      ================================================================================
      ================================================================================
      Installation

      1) [x] Language settings                 2) [x] Time settings
             (English (United States))                (America/Chicago timezone)
      3) [!] Installation source               4) [!] Software selection
             (Setting up installation                 (Processing...)
             source...)
      5) [!] Installation Destination          6) [x] Network configuration
             (Processing...)                          (Connected: eth0)
      7) [!] Root password                     8) [!] User creation
             (Root account is disabled)               (No user will be created)

      Please make a selection from the above ['b' to begin installation, 'h' to help,
      'q' to quit, 'r' to refresh]:

   For ``3) [!] Installation source``, enter ``3``, then ``1`` to select
   ``CD/DVD``.

   .. code-block:: console

      ================================================================================
      ================================================================================
      Installation source

      Choose an installation source type.
      1) CD/DVD
      2) local ISO file
      3) Network

      Please make a selection from the above ['c' to continue, 'h' to help, 'q' to
      quit, 'r' to refresh]: 1

   For ``4) [!] Software selection``, enter ``4``, then ``c`` to continue.

   For ``5) [!] Installation Destination``, enter ``5``, then ``c`` to select
   the default options.

   For ``6) [!] Network configuration``, it will automatically change to ``x``.

   For ``7) [!] Root password``, follow the prompts to enter the password and
   confirm.

   After entering root password, ``8) [ ] User creation`` becomes optional and
   can be skipped.

   The final configuration should now appear as follows:

   .. code-block:: console

      ================================================================================
      ================================================================================
      Installation

      1) [x] Language settings                 2) [x] Time settings
             (English (United States))                (America/Chicago timezone)
      3) [x] Installation source               4) [x] Software selection
             (Local media)                            (Fedora Server Edition)
      5) [x] Installation Destination          6) [x] Network configuration
             (Automatic partitioning                  (Connected: eth0)
             selected)
      7) [x] Root password                     8) [ ] User creation
             (Root password is set)                   (No user will be created)

      Please make a selection from the above ['b' to begin installation, 'h' to help,
      'q' to quit, 'r' to refresh]: 

   Now enter ``b`` to start the installation.

4. The installer is expected to stay at ``Configuring kernel-core.aarch64``
   for several hours. The installer will then verify the installed packages
   and continue to install the boot loader.
5. The following error is expected while installing the boot loader. Ignore the
   error by responding ``yes`` and continue.

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

* Log in

  When the installation reaches the final ``Finishing the installation``
  phase, you will need to wait some time to finish the remaining tasks,
  and then it will automatically reboot into the installed OS. You can log into
  the Linux shell with the user created during installation.

* Terminate the FVP

  To shut down the FVP and terminate the emulation automatically, log into the
  Linux shell as the root user then run the following command.

      .. code-block:: text

         shutdown now

  The below message indicates the shutdown process is complete.

      .. code-block:: text

         reboot: Power down

  Subsequently running the FVP will boot into Fedora.

.. _user_guide_reproduce_secure_firmware_update:

Secure Firmware Update
======================

The firmware update is on-disk to meet Arm SystemReady IR requirements.

Baremetal Architecture
----------------------

Build
^^^^^

The firmware update capsule for testing will be generated together with
the image for the software stack when building. The firmware capsule is placed
on a removable storage device (in the case of Arm FVPs, an MMC card
implementation).

Run this step to ensure the creation of the initial firmware flash images:


.. code-block:: text

  kas shell -c "bitbake firmware-fvp-rd-kronos -C deploy"

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To build a Baremetal Architecture image:

1. Select ``Critical Application Monitoring Demo`` from the ``Use-Case`` menu.
2. Select ``Baremetal`` from the ``Reference Software Stack Architecture`` menu.
3. Select ``Build``.

Run the FVP
^^^^^^^^^^^

To start the FVP and connect to the Primary Compute terminal (running Linux):

.. code-block:: text

  kas shell -c "../layers/meta-arm/scripts/runfvp -t tmux --verbose"

Note that the main tmux windows involved in the Secure Firmware Update are
``terminal_ns_uart0`` and ``terminal_rse_uart``. For ease of navigation, it is
recommended to join these in a single window with two panes.

Follow the steps below to achieve the same:

 1. Ensure that the tmux window titled ``terminal_ns_uart0`` is selected.
    If not, press ``Ctrl-b w`` from the tmux session, navigate to the tmux
    window titled ``terminal_ns_uart0`` followed by pressing the ``Enter`` key.
 2. Press ``Ctrl-b :`` and then type ``join-pane -s :terminal_rse_uart -h``
    followed by pressing the ``Enter`` key to join the RSE terminal window to
    the Primary Compute terminal window.

Refer to the following image of the tmux panes rearrangement. Panes can
be navigated using ``Ctrl-b`` followed by the arrow keys.

  .. image:: ../images/secure_firmware_update_baremetal_fvp.*
    :align: center
    :alt: Arm Auto Solutions Secure Firmware Update Baremetal FVP Windows

|

The user should wait for the system to boot and for the Linux prompt to appear.

The Reference Software Stack running on the Primary Compute can be logged into
as the ``root`` user without a password in the Linux terminal. Run the below
command to guarantee that all the expected services have been initialized.

.. code-block:: text

  systemctl is-system-running --wait

Wait for it to return. The expected terminal output is ``running``.


Run the Demo
^^^^^^^^^^^^

1. To start the Secure Firmware Update, run the following commands to create
   the UpdateCapsule directory and copy the update capsule into it, followed by
   a reboot.

   .. code-block:: text

      mount /dev/vda1 /boot
      mount /dev/mmcblk0p1 /mnt
      mkdir -p /boot/EFI/UpdateCapsule
      cp -f /mnt/fw.cap /boot/EFI/UpdateCapsule
      reboot

   The system will begin the firmware update automatically. The following logs
   indicate that the upgrade process has started and is in progress.

   In ``terminal_ns_uart0``:

   .. code-block:: text

      FF-A driver 1.0
      FF-A framework 1.0
      FF-A versions are compatible
      EFI: MM partition ID 0x8003
      EFI: FVP: Capsule shared buffer at 0x81000000 , size 8192 pages

   In ``terminal_rse_uart``:

   .. code-block:: text

      [INF]:[FWU]: get_fwu_agent_state: enter, boot_index = 0
      [INF]:[FWU]: get_fwu_agent_state: enter, boot_index = 0
      [INF]:[FWU]: FMP image update: image id = 1
      [INF]:[FWU]: FMP image update: status = 0, version=7, last_attempt_version=0.
      [INF]: [FWU]: Host acknowledged.
      [INF]:[FWU]: pack_image_info:207 ImageInfo size = 105, ImageName size = 14, ImageVersionName size = 14
      [INF]: [FWU]: Getting image info succeeded.
      [INF]:[FWU]: get_fwu_agent_state: enter, boot_index = 0
      [INF]:[FWU]: uefi_capsule_retrieve_images: enter, capsule ptr = 0x0x65000000
      [INF]:[FWU]: uefi_capsule_retrieve_images: capsule size = 18284656, image count = 1
      [INF]:[FWU]: uefi_capsule_retrieve_images: image 0, version = 3
      [INF]:[FWU]: uefi_capsule_retrieve_images: image 0 at 0x65000070, size=18284560
      [INF]:[FWU]: flash_rss_capsule: enter: image = 0x65000070, size = 16187408, version = 3
      [INF]:[FWU]: erase_bank: erasing sectors = 4080, from offset = 16748544
      [INF]:[FWU]: flash_rss_capsule: writing capsule to the flash at offset = 16748544...

   .. note::
      This step will take about 10 minutes.

2. The system will reset after a successful firmware update and boot with the
   updated firmware. This can be confirmed by checking the terminal logs; if
   there are lines in the log like below, then the upgrade was successful and
   the system has successfully rebooted with the updated firmware.

   In ``terminal_rse_uart``:

   .. code-block:: text

      [INF]: [FWU]: Flashing the image succeeded.
      [INF]: [FWU]: Performing system reset...
      ...
      ...
      [INF]:[FWU]: get_fwu_agent_state: enter, boot_index = 1

3. The system will eventually boot into Linux using the upgraded firmware.

4. To shut down the FVP and terminate the emulation automatically, follow the
   below steps:

    * Issue the following command on the Primary Compute terminal.

      .. code-block:: text

         shutdown now

      The below messages indicate the shutdown process is complete.

      .. code-block:: text

         [  OK  ] Finished System Power Off.
         [  OK  ] Reached target System Power Off.
         reboot: Power down

    * Close the tmux pane started for the build host machine by pressing
      ``Ctrl-d``.

Automated Validation
^^^^^^^^^^^^^^^^^^^^

Run this step to ensure the creation of the initial firmware flash images:


.. code-block:: text

  kas shell -c "bitbake firmware-fvp-rd-kronos -C deploy"

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To run the validation tests:

  1. Select ``Critical Application Monitoring Demo`` as ``Use-Case``.
  2. Select ``Baremetal`` from the ``Reference Software Stack Architecture``
     menu.
  3. Select ``Run Automated Validation`` from the ``Runtime Validation Setup``
     menu.
  4. Select ``Build``.

The following messages are expected in the output to validate this Use-Case:

.. code-block:: text

  RESULTS - test_00_fwu.SecureFirmwareUpdateTest.test_securefirmwareupdate: PASSED (414.85s)

.. note::
    There is a rare known failure where a timeout might occur during test execution.
    
    Refer to :ref:`releasenotes_knownissues` for possible workarounds.

See :ref:`validation_secure_firmware_update` for more details.

Virtualization Architecture
---------------------------

Build
^^^^^

The firmware update capsule for testing will be generated together with
the image for the software stack when building. The firmware capsule is placed
on a removable storage device (in the case of Arm FVPs, an MMC card
implementation).

Run this step to ensure the creation of the initial firmware flash images:


.. code-block:: text

  kas shell -c "bitbake firmware-fvp-rd-kronos -C deploy"

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To build a Virtualization Architecture image:

1. Select ``Critical Application Monitoring Demo`` from the ``Use-Case`` menu.
2. Select ``Virtualization`` from the ``Reference Software Stack Architecture``
   menu.
3. Select ``Build``.

Run the FVP
^^^^^^^^^^^

To start the FVP and connect to the Primary Compute terminal (running Linux):

.. code-block:: text

  kas shell -c "../layers/meta-arm/scripts/runfvp -t tmux --verbose"

Note that the main tmux windows involved in the Secure Firmware Update are
``terminal_ns_uart0`` and ``terminal_rse_uart``. For ease of navigation, it is
recommended to join these in a single window with two panes.

Follow the steps below to achieve the same:

 1. Ensure that the tmux window titled ``terminal_ns_uart0`` is selected.
    If not, press ``Ctrl-b w`` from the tmux session, navigate to the tmux
    window titled ``terminal_ns_uart0`` followed by pressing the ``Enter`` key.
 2. Press ``Ctrl-b :`` and then type ``join-pane -s :terminal_rse_uart -h``
    followed by pressing the ``Enter`` key to join the RSE terminal window to
    the Primary Compute terminal window.

Refer to the following image of the tmux panes rearrangement. Panes can
be navigated using ``Ctrl-b`` followed by the arrow keys.

  .. image:: ../images/secure_firmware_update_virtualization_fvp.*
    :align: center
    :alt: Arm Auto Solutions Secure Firmware Update Virtualization FVP Windows

|

The user should wait for the system to boot and for the Linux prompt to appear.

The Reference Software Stack running on the Primary Compute can be logged into
as the ``root`` user without a password in the Linux terminal. Run the below
command to guarantee that all the expected services have been initialized.

.. code-block:: text

  systemctl is-system-running --wait

Wait for it to return. The expected terminal output is ``running``.


Run the Demo
^^^^^^^^^^^^

1. To start the Secure Firmware Update, run the following commands to create
   the UpdateCapsule directory and copy the update capsule into it, followed by
   a reboot.

   .. code-block:: text

      mount /dev/vda1 /boot
      mount /dev/mmcblk0p1 /mnt
      mkdir -p /boot/EFI/UpdateCapsule
      cp -f /mnt/fw.cap /boot/EFI/UpdateCapsule
      reboot

   The system will begin the firmware update automatically. The following logs
   indicate that the upgrade process has started and is in progress.

   In ``terminal_ns_uart0``:

   .. code-block:: text

      FF-A driver 1.0
      FF-A framework 1.0
      FF-A versions are compatible
      EFI: MM partition ID 0x8003
      EFI: FVP: Capsule shared buffer at 0x81000000 , size 8192 pages

   In ``terminal_rse_uart``:

   .. code-block:: text

      [INF]:[FWU]: get_fwu_agent_state: enter, boot_index = 0
      [INF]:[FWU]: get_fwu_agent_state: enter, boot_index = 0
      [INF]:[FWU]: FMP image update: image id = 1
      [INF]:[FWU]: FMP image update: status = 0, version=0, last_attempt_version=0.
      [INF]: [FWU]: Host acknowledged.
      [INF]:[FWU]: pack_image_info:207 ImageInfo size = 105, ImageName size = 14, ImageVersionName size = 14
      [INF]: [FWU]: Getting image info succeeded.
      [INF]:[FWU]: get_fwu_agent_state: enter, boot_index = 0
      [INF]:[FWU]: uefi_capsule_retrieve_images: enter, capsule ptr = 0x0x65000000
      [INF]:[FWU]: uefi_capsule_retrieve_images: capsule size = 18219116, image count = 1
      [INF]:[FWU]: uefi_capsule_retrieve_images: image 0, version = 3
      [INF]:[FWU]: uefi_capsule_retrieve_images: image 0 at 0x6500006c, size=18219024
      [INF]:[FWU]: flash_rss_capsule: enter: image = 0x6500006c, size = 16121872, version = 3
      [INF]:[FWU]: erase_bank: erasing sectors = 4080, from offset = 16748544
      [INF]:[FWU]: flash_rss_capsule: writing capsule to the flash at offset = 16748544...

   .. note::
      This step will take about 10 minutes.

2. The system will reset after a successful firmware update and boot with the
   updated firmware. This can be confirmed by checking the terminal logs; if
   there are lines in the log like below, then the upgrade was successful and
   the system has successfully rebooted with the updated firmware.

   In ``terminal_rse_uart``:

   .. code-block:: text

      [INF]: [FWU]: Flashing the image succeeded.
      [INF]: [FWU]: Performing system reset...
      ...
      ...
      [INF]:[FWU]: get_fwu_agent_state: enter, boot_index = 1

3. The system will eventually boot into Linux using the upgraded firmware.

4. To shut down the FVP and terminate the emulation automatically, follow the
   below steps:

    * Issue the following command on the Primary Compute terminal.

      .. code-block:: text

         shutdown now

      The below messages indicate the shutdown process is complete.

      .. code-block:: text

         [  OK  ] Finished System Power Off.
         [  OK  ] Reached target System Power Off.
         reboot: Power down

    * Close the tmux pane started for the build host machine by pressing
      ``Ctrl-d``.

Automated Validation
^^^^^^^^^^^^^^^^^^^^

Run this step to ensure the creation of the initial firmware flash images:


.. code-block:: text

  kas shell -c "bitbake firmware-fvp-rd-kronos -C deploy"

To run the configuration menu:

.. code-block:: text

  kas menu sw-ref-stack/Kconfig

To run the validation tests:

  1. Select ``Critical Application Monitoring Demo`` as ``Use-Case``.
  2. Select ``Virtualization`` from the ``Reference Software Stack Architecture``
     menu.
  3. Select ``Run Automated Validation`` from the ``Runtime Validation Setup``
     menu.
  4. Select ``Build``.

The following messages are expected in the output to validate this Use-Case:

.. code-block:: text

  RESULTS - test_00_fwu.SecureFirmwareUpdateTest.test_securefirmwareupdate: PASSED (414.85s)

.. note::
    There is a rare known failure where a timeout might occur during test execution.
    
    Refer to :ref:`releasenotes_knownissues` for possible workarounds.

See :ref:`validation_secure_firmware_update` for more details.

..
 # SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
 # affiliates <open-source-office@arm.com></text>
 #
 # SPDX-License-Identifier: MIT

.. _design_applications_cam:

####################################
Critical Application Monitoring Demo
####################################

************
Introduction
************

Critical applications often follow a pattern where the workloads are split into
multiple periodic tasks chained together to produce a feature pipeline.
Detection of application execution faults in such safety-critical systems is one
of the pillars of a system’s reliability strategy. The Critical Application
Monitoring (CAM) project implements a solution for monitoring such critical
applications using a monitoring service that runs on a higher safety level
system. The main goal of CAM is to ensure that a certain piece of code running
in critical applications executes periodically at a specific frequency. When
the execution time is violated, critical applications are deemed as
malfunctioning. The classes of issues that CAM can detect can be broadly
classified into:

  * Temporal issues: Events arriving outside the expected frequency.
  * Logical issues: Events arriving out of order.

The CAM project is integrated into the Kronos Reference Software Stack to
demonstrate the feasibility of monitoring Primary Compute applications from the
Safety Island. Refer to `Critical Application Monitoring Documentation`_ for
more information on CAM project and its implementation details.

*****************************************
Critical Application Monitoring on Kronos
*****************************************

The Critical Application Monitoring demo can be run on both Baremetal and
Virtualization Architectures.

The following diagram shows the architecture of the demo in the Baremetal
Architecture:

|

.. image:: ../../images/critical_application_monitoring.*
   :align: center
   :alt: Critical Application Monitoring Demo High-Level Diagram

|

CAM consists of the following major components:

  * **Stream configuration file**: Configuration file containing the number of
    stream events and their timing characteristics according to the requirements
    of the critical application.

  * **Stream deployment data**: Binary representation of the stream
    configuration that needs to be deployed to the Safety Island.

  * **cam-tool**: A python-based tool used to generate and deploy stream
    deployment data by analyzing stream configuration file.

  * **cam-service**: CAM monitoring agent that monitors event streams sent by
    critical applications and runs from higher safety cores in the Safety
    Island. ``cam-service`` uses the stream deployment data to validate event
    streams produced by critical applications.

  * **libcam**: CAM library that offers a simple, thread-safe API that can be
    used by critical applications to integrate the CAM project. The API enables
    the applications to register with ``cam-service`` and generate event streams
    to be sent to ``cam-service``.

  * **cam-app-example**: An example application that uses ``libcam`` API to
    integrate CAM framework. It also supports error injection into the stream
    events to trigger a fault detection by ``cam-service``.

The Primary Compute components are deployed on the baremetal Linux root
filesystem in the Baremetal Architecture build and on the DomU1 and DomU2 Linux
root filesystem in the Virtualization Architecture.

In the Kronos Reference Software Stack, ``cam-service`` is deployed on the
Safety Island Cluster 1 in order to provide applications on the Primary Compute
with a high safety level of monitoring services.

The following are platform requirements to support the ``cam-service``
deployment on the Safety Island:

  * Communication between the Safety Island and the Primary Compute for event
    streams.
  * Synchronized clocks on the Safety Island and the Primary Compute for
    temporal check.
  * Storage and a file system on the Safety Island for stream data deployment.

Virtualization Architecture
===========================

The following diagram shows the architecture of the demo in the Virtualization
Architecture:

|

.. image:: ../../images/critical_application_monitoring_virtualization.*
   :align: center
   :alt: Critical Application Monitoring Demo High-Level Diagram Virtualization

|

In this deployment, two different instances of **cam-app-example** run on
DomU1 and DomU2. Each application is monitored by **cam-service** concurrently
via separate data deployment and event streams.

Communication Interfaces
========================

BSD sockets (over TCP) are used in order to send the event message from
``cam-app-example`` to ``cam-service`` via the :ref:`design_hipc` feature.

Time Synchronization
====================

Real-time clocks on the Primary Compute and the Safety Island are synchronized
via the :ref:`hipc_network_topology_gptp` protocol.

Zephyr File System
==================

Zephyr supports the FAT file system and can mount it to a RAM disk.
Refer to `Zephyr file system`_.

.. note::
  Due to the volatility of the RAM disk, on every system boot, the CAM stream
  data needs to be deployed from the Primary Compute to the Safety Island
  Cluster 1 via ``cam-tool``.

Validation
==========

Refer to the CAM Demo validations :ref:`validation_cam_tests`.

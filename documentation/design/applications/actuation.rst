..
 # SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
 # affiliates <open-source-office@arm.com></text>
 #
 # SPDX-License-Identifier: MIT

.. _design_applications_actuation:

############################
Safety Island Actuation Demo
############################

************
Introduction
************

The Safety Island Actuation Demo is an example application that shows how an
Autonomous Drive software stack can be run in a compute environment composed of
a high-performance Primary Compute platform coupled with a higher reliability
Safety Island.

The Safety Island Actuation Demo features an "Actuation Service" application
running on the Safety Island that receives inputs from the Primary Compute and
generates control commands that can be passed to an actuation system. A
reference implementation is provided by the `Safety Island Actuation Demo`_. The
software running on the Primary Compute is an `Autoware`_ pipeline and the
"Actuation Service" takes the form of a Zephyr application showcasing the `Pure
Pursuit`_ algorithm from Autoware.Auto. The two communicate via `Data Distribution
Service`_ (DDS) messages over a network interface.

**************************************
Safety Island Actuation Demo on Kronos
**************************************

Compared to the default deployment of the `Safety Island Actuation Demo`_, the
Kronos deployment has, on the Primary Compute, an "Actuation Player" component
instead of the Autoware pipeline and, on the Host, a "Packet Analyzer" instead
of a visualization software. This is done in order to minimize the load for an
FVP target.

This demo can run on both the Baremetal and Virtualization Architectures. In
case of the Virtualization Architecture, the "Actuation Player" is deployed on
DomU1.

The following diagram describes the data flow of the demo:

|

.. image:: ../../images/actuation.*
   :align: center
   :alt: Safety Island Actuation Demo High-Level Diagram

|

For more information on the underlying network topology, and the description of
the network bridging involved in providing access to the external network, see
the :ref:`hipc_network_topology` section.

Main Components
===============

The Actuation Demo on Kronos has 3 components:

* Actuation Player

  * Plays a recording of a driving scenario from the default deployment of the
    Actuation Demo

  * Recipe at
    :kronos-repo:`yocto/meta-kronos/recipes-demos/actuation/actuation-player_2.0.0.bb`

* Actuation Service

  * Functionally the same as the "Actuation Service" from the default deployment
    of the Actuation Demo, except that the computed commands are sent through a
    BSD socket instead of a DDS connection

  * Recipe at
    :kronos-repo:`yocto/meta-kronos/recipes-kernel/zephyr-kernel/zephyr-actuation.bb`

  * Zephyr overlays at:

    * :kronos-repo:`components/safety_island/zephyr/src/overlays/hipc/fvp_rd_kronos_safety_island_c2.conf`

    * :kronos-repo:`components/safety_island/zephyr/src/overlays/hipc/fvp_rd_kronos_safety_island_c2.overlay`

    * :kronos-repo:`components/safety_island/zephyr/src/apps/actuation/boards/fvp_rd_kronos_safety_island_c2_actuation.conf`

* Packet Analyzer

  * Checks for correctness of the "Actuation Service" output

  * Recipe at
    :kronos-repo:`yocto/meta-kronos/recipes-demos/actuation/packet-analyzer-native_2.0.0.bb`

Communication Interfaces
========================

Actuation Player <> Actuation Service
-------------------------------------

`CycloneDDS`_ (Using a specific upstream commit located at
:kronos-repo:`yocto/meta-kronos/recipes-demos/actuation/cyclonedds_0.10.3.inc`)
is used for the communication between the "Actuation Player" and the "Actuation
Service".

Actuation Service <> Packet Analyzer
------------------------------------

BSD socket (TCP Protocol) is used in order to send the Control Commands from the
"Actuation Service" to the "Packet Analyzer".

Validation
==========

Refer to the Actuation Demo validations :ref:`validation_actuation_demo`

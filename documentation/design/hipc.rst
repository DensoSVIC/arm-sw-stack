..
 # SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
 # affiliates <open-source-office@arm.com></text>
 #
 # SPDX-License-Identifier: MIT

.. _design_hipc:

##################################################
Heterogeneous Inter-Processor Communication (HIPC)
##################################################

************
Introduction
************

The FVP contains Armv9-A (Primary Compute) and Armv8-R64 (Safety Island)
heterogeneous processing elements which share data via the Message Handling
Unit (MHUv3) and shared Static Random Access Memory (SRAM). The MHUv3 is a
mailbox controller used for signal transmission and the shared memory is used
for data exchange. Safety Island clusters also share data via the MHUv3 and
shared SRAM.

The HIPC demonstrates the communication between:

  * Primary Compute and the three Safety Island clusters.
  * Safety Island clusters.

**************************************************************************
Communication between Primary Compute and Safety Island clusters
**************************************************************************

|

.. image:: ../images/hipc_baremetal_design.*
   :align: center
   :alt: Arm Automotive Solutions HIPC - Baremetal Architecture

|

RPMsg Protocol
==============

RPMsg (Remote Processor Messaging) is a messaging protocol enabling
heterogeneous communication, which can be used by Linux as well as Real Time
Operating Systems.

In Linux, the RPMsg framework is implemented on top of the Virtio-RPMsg
bus and Remoteproc framework. The Virtio-RPMsg implementation is generic and
based on Virtio Vring to transmit/receive messages to/from the remote CPU over
shared memory.

On the Safety Island side, Zephyr has imported OpenAMP as an external module.
The OpenAMP library implements the RPMsg backend based on Virtio, which is
compatible with the upstream Linux Remoteproc and RPMsg components. This library
can be used with the Zephyr kernel or Zephyr applications to behave as an RPMsg
backend service for communication with the Primary Compute.

Virtual Network Device over RPMsg
=================================

RPMsg offers a range of user APIs for RPMsg endpoints to send and receive messages
to and from these endpoints. These APIs are suitable for simple inter-processor
communication. However, many current user applications are not built on RPMsg APIs.
Instead, they use BSD sockets for IPC. The reason for this is that BSD sockets can
abstract the difference between inter-processor communication and intra-processor
communication. This makes it possible for applications to be more versatile and
portable. In response to the needs of such applications, a virtual network device
based on RPMsg has been added to the Reference Software Stack.

On the Safety Island side, a network device is created over an RPMsg endpoint
with a specific service name. The RPMsg endpoint sends a Name Service message
to the Primary Compute to announce its existence. The message is then processed
by the RPMsg bus, which creates an RPMsg endpoint and a corresponding network
device. Once this is done, the virtual network devices establish network
communication.

On the Primary Compute side RPMsg frame must be copied to the Socket Buffer (skb)
utilized by the Network Stack. However, if the traffic exceeds the performance
limit, the Socket Buffer may get dropped during processing for congestion control
or by the protocol layers. In such cases, the network statistics will increase
the dropped packet counter.

In the above diagram, each Safety Island cluster has its own Shared Memory and
MHUv3 device to communicate with the Primary Compute. The size of the Shared
Memory is 16MB, and Safety Island Clusters 0, 1, and 2 have access to it. The
Shared Memory instance has a Resource table (4KB), Vring 0, 1 (1MB each), and
an RPMsg vbuffer (3MB) used to send and receive information between the Primary
Compute and the Safety Island cluster.

On the Primary Compute, the Safety Island Remoteproc driver and RPMsg-based virtual
interface driver are added to communicate with the Safety Island. The RPMsg-net
driver on the Primary Compute and Veth-RPMsg on the Safety Island clusters implement
the virtual ethernet device that is the basis for communication between the Primary
Compute and Safety Island clusters.

Safety Island Remoteproc Driver
===============================
The Remoteproc framework allows different platforms/architectures to control
(power on/off, load firmware) remote processors while abstracting the hardware
differences, so the entire driver doesn't need to be duplicated. The Remoteproc
platform driver is added to the software stack to provide support for
communication between Primary Compute and Safety Island clusters.

In the FVP, Linux running in the Primary Compute, regards the Safety Island
clusters as its remote processors. The Safety Island has three clusters. Each
cluster behaves as an independent entity and has its own resources to establish
the connection to the Primary Compute.

These clusters cannot be booted by the Primary Compute processor because they
need to monitor the other hardware, including the Primary Compute. Therefore,
the initial status of the clusters in the driver is ``RPROC_DETACHED``, which
means the cluster has been booted independently from the Primary Compute
processor. This driver implements the notification handler using an MHUv3 based
mailbox, which notifies other cores when new messages are sent to the virtual
queue.

The Resource table, Vring 0, 1, and RPMsg vbuffer memory regions are set up in the
device tree bindings for each cluster. The driver reads the device tree node for
each cluster and adds it to the Remoteproc framework. Each cluster has its own
Resource table, Vring 0, 1, and RPMsg vbuffer, which serve as the foundation for
communication.

Virtualization Architecture
===========================

In the Virtualization Architecture of the Reference Software Stack, virtual
network interfaces based on Xen drivers created in the control domain (Dom0) are
exposed to the domUs. These virtual network interfaces are added to an Open
vSwitch virtual switch along with an RPMsg Virtual Interface to communicate
with the Safety Island.

Dom0 has a communication channel with the Safety Island which is the same as
the Baremetal Architecture.

|

.. image:: ../images/hipc_virtualization_design.*
   :align: center
   :alt: Arm Automotive Solutions HIPC - Virtualization Architecture

|

There are some limitations of the virtual network device over RPMsg.
Refer to the release notes :ref:`releasenotes_limitations` section.

************************************************
Communication between the Safety Island clusters
************************************************

Virtual Network Device over IPC Static Vrings
=============================================

Zephyr `IPC Service`_  based virtual network devices are added to each cluster
to provide communication between clusters via BSD sockets. The backend used for
the IPC service is RPMSg Static Vrings. The IPC RPMsg Static Vrings backend is
implemented on top of Virtio based RPMsg communication.

|

.. image:: ../images/hipc_inter_si_communication_design.*
   :align: center
   :alt: Inter-Safety Island Clusters Communication

|

.. _hipc_network_topology:

**********
Memory Map
**********

The dedicated SRAM used by the Primary Compute and Safety Island Clusters 0, 1,
and 2 for inter-processor data transfer has the following memory regions:
**Resource table**, **Vring0**, **Vring1**, and **Virtio Buffer**.

Safety Island side:
===================

 Cluster 0:

  Primary Compute <-> Cluster 0:

  * ``rsc_table`` : Used to share resource information between Primary
    Compute and Cluster 0
  * ``shared_data`` : Used for data transfer between Primary Compute  and
    Cluster 0

  Cluster 0 <-> Cluster 1, 2:

  * ``local_sram_cl0_cl1`` : Used for data transfer between Cluster 0
    and Cluster 1
  * ``local_sram_cl0_cl2`` : Used for data transfer between Cluster 0
    and Cluster 2

  Refer to the device tree overlay below for more information about the memory
  addresses and region sizes.

   * :repo:`components/safety_island/zephyr/src/overlays/hipc/fvp_rd_kronos_safety_island_c0.overlay`.

 Cluster 1:

  Primary Compute <-> Cluster 1:

  * ``rsc_table`` : Used to share resource information between Primary
    Compute and Cluster 1
  * ``shared_data`` : Used for data transfer between Primary Compute  and
    Cluster 1

  Cluster 1 <-> Cluster 0, 1:

  * ``local_sram_cl1_cl0`` : Used for data transfer between Cluster 1
    and Cluster 0
  * ``local_sram_cl1_cl2`` : Used for data transfer between Cluster 1
    and Cluster 2

  Refer to the device tree overlay below for more information about the memory
  addresses and region sizes.

   * :repo:`components/safety_island/zephyr/src/overlays/hipc/fvp_rd_kronos_safety_island_c1.overlay`.

 Cluster 2:

  Primary Compute <-> Cluster 2:

   * ``rsc_table`` : Used to share resource information between Primary Compute
     and Cluster 2
   * ``shared_data`` : Used for data transfer between Primary Compute  and
     Cluster 2

  Cluster 2 <-> Cluster 0, 2:

   * ``local_sram_cl2_cl0`` : Used for data transfer between Cluster 2
     and Cluster 0
   * ``local_sram_cl2_cl1`` : Used for data transfer between Cluster 2
     and Cluster 1

  Refer to the device tree overlay below for more information about the memory
  addresses and region sizes.

   * :repo:`components/safety_island/zephyr/src/overlays/hipc/fvp_rd_kronos_safety_island_c2.overlay`.

Primary Compute side:
=====================

   * ``si_c0_rproc_rsctbl`` : Used to share resource information between Primary
     Compute and Cluster 0
   * ``si_c0_vdev0vring0`` : Primary Compute vring, used to pass messages from
     Cluster 0 to Primary Compute
   * ``si_c0_vdev0vring1`` : Safety Island Cluster 0 vring, used to pass messages
     from Primary Compute to Cluster 0
   * ``si_c0_vdev0buffer`` : Used for data transfer between Primary Compute and Cluster 0
   * ``si_c1_rproc_rsctbl`` : Used to share resource information between Primary Compute
     and Cluster 1
   * ``si_c1_vdev0vring0`` : Primary Compute vring, used to pass messages from
     Cluster 1 to Primary Compute
   * ``si_c1_vdev0vring1`` : Safety Island Cluster 1 vring, used to pass messages from
     Primary Compute to Cluster 1
   * ``si_c1_vdev0buffer`` : Used for data transfer between Primary Compute and Cluster 1
   * ``si_c2_rproc_rsctbl`` : Used to share resource information between Primary Compute
     and Cluster 2
   * ``si_c2_vdev0vring0`` : Primary Compute vring, used to pass messages from Cluster 2
     to Primary Compute
   * ``si_c2_vdev0vring1`` : Safety Island Cluster 2 vring, used to pass messages from
     Primary Compute to Cluster 2
   * ``si_c2_vdev0buffer`` : Used for data transfer between Primary Compute and Cluster 2

  Refer to the device tree below for more information about the memory address and
  region size.

   * :meta-arm-repo:`meta-arm-bsp/recipes-bsp/trusted-firmware-a/files/fvp-rd-kronos/rdkronos.dts`.

****************
Network Topology
****************

VLAN
====

`Open vSwitch`_ is used to create a virtual switch that connects all the
network interfaces of the Primary Compute.

VLAN is a concept standardized by `IEEE 802.1Q`_. It is used to partition a
switch into multiple logical switches. The VLAN tag has a value from 0 to 4096
stored in the packet header. Usually 0 means that the packet is untagged, but
some values are reserved.

On a switch, using VLAN tagged traffic makes sure that a packet tagged with a
certain VLAN identifier reaches only ports that are configured to manage the
traffic tagged with that identifier (tag).

The traffic between the Primary Compute and the Safety Island is using the
following VLAN identifiers:

 * VLAN **100**: Traffic from/to **Safety Island Cluster 0**
 * VLAN **200**: Traffic from/to **Safety Island Cluster 1**
 * VLAN **300**: Traffic from/to **Safety Island Cluster 2**

.. _hipc_network_topology_gptp:

gPTP
====

Generalized Precision Time Protocol (gPTP) is a concept standardized by `IEEE
802.1AS`_. It is used to synchronize the clocks of multiple systems over a
network. A "PTP Instance" is an instance of this protocol. Each PTP Instance can
have one or more logical access point to the network (a "PTP Port"). The source
of the synchronized time in a domain is a single PTP Instance, the "Grandmaster
PTP Instance", which always act as a server.

In Arm Automotive Solutions, Grandmaster PTP Instances are deployed on the
Primary Compute (in Dom0 in case of the Virtualization Architecture),
advertizing a single source of time to the other PTP Instances (on the Safety
Island clusters and the DomUs) acting as clients. The Grandmaster PTP Instances
each have one PTP Port per remote PTP Instance. All the Operating Systems that
make use of gPTP have a dedicated service to handle the network messages:

* On Linux, the `Linux PTP Project`_ provides a ``ptp4l`` program that creates a
  PTP Port on a specified network interface. At system boot, one ``ptp4l``
  daemon is started per network interface specified in the ``LINUXPTP_IFACES``
  bitbake variable. This variable is set per :ref:`Use-Case
  <overview_use_cases>`, with the Safety Island Communication Demo Use-Case
  making use of gPTP on all Operating Systems. The network interfaces created by
  Open vSwitch are not capable of software timestamping; hence, the direct
  network interfaces to the remote participant are used instead (for example for
  Safety Island Cluster 0, ``ptp4l`` binds to ethsi0, not brsi0). Note that
  ``ptp4l`` only writes to the system logger, not to the console, including in
  case of de-synchronization.

* On Zephyr, the kernel provides a `Zephyr gPTP subsystem`_. Enabling it is done
  per application, by including the appropriate configuration file from
  :repo:`components/safety_island/zephyr/src/overlays/gptp`. They disable
  the Grandmaster capability and create a single PTP Port, on the first network
  interface. When the client is not synchronized with the server, the gPTP
  subsystem prints a warning-level logging message (``<wrn> net_gptp: Reset
  Pdelay requests``) at each tick of its state machine (about once per second).

In Arm Automotive Solutions, all of the PTP Instances use software timestamping.
This limits the maximum achievable precision of the clock synchronization and it
makes the stability of the clock vulnerable to software activity on either
side of the gPTP link.

See :ref:`validation_gptp` for details on how the functionality is validated.

External Connection
===================

The Safety Island has a single network interface leading outside the FVP system
located on Cluster 0.

A software-based network bridge deployed on Cluster 0 bridges this external
interface with the IPC channels to the other Safety Island clusters so Cluster
1 and 2 can reach outside FVP.

See :ref:`design_applications_bridge` for more information.

Baremetal Architecture
======================

This diagram shows the network topology for the Baremetal Architecture. ethsi{N}
is the name of the RPMsg-based Virtual Interfaces that are connected to Safety
Island Cluster{N}, where N is the cluster number. For example, the ethsi0
interfaces are connected to Safety Island Cluster 0. Similarly, ethpc is the
name of the interfaces that are connected to the Primary Compute.

ovsbr0 is the Open vSwitch network switch which carries untagged traffic. The
communication between the Primary Compute and Safety Island is managed through
the brsi{N} VLAN tagged switches that are configured to carry VLAN tagged
traffic from/to the ethsi{N} interface with the Safety Island.

User space applications on the Primary Compute can communicate with Safety
Island Cluster N via brsi{N}.

|

.. image:: ../images/hipc_network_topology_baremetal.*
   :align: center
   :alt: Arm Automotive Solutions Network Topology - Baremetal Architecture

|

Virtualization Architecture
===========================

As shown in the diagram below the virtual network interfaces for the Xen guests
are based on Xen drivers. domu1.ethsi{N} and domu2.ethsi{N} are backend virtual
network interfaces that are exposed to DomU1 and DomU2 guests. ethsi{N} in the
Primary Compute is the RPMsg-based Virtual Interface that is connected to
Safety Island Cluster{N} to provide communication between Primary Compute and
Safety Island. ethsi{N}(Primary Compute) and domu1.ethsi{N} are added to
Open vSwitch (brsi{N}) to have a connection between Dom0, DomU1 and Safety
Island Cluster N.

|

.. image:: ../images/hipc_network_topology_virtualization.*
   :align: center
   :alt: Arm Automotive Solutions Network Topology - Virtualization Architecture

|

***********
Device Tree
***********

In Linux, a Remoteproc binding is needed for Safety Island clusters.
It includes MHUv3 transmit/receive channels for signaling and several memory
regions for data exchange. Each Safety Island cluster has it own Remoteproc
binding that includes MHUv3 and Shared Memory.

The Linux device tree with the appropriate nodes for HIPC is located at
:meta-arm-repo:`meta-arm-bsp/recipes-bsp/trusted-firmware-a/files/fvp-rd-kronos/rdkronos.dts`.

In Zephyr, there is an overlay device tree for the network over RPMsg application,
which also defines the MHUv3 channels and device memory regions.

The Zephyr overlay device tree for HIPC is located at
:repo:`components/safety_island/zephyr/src/overlays/hipc`.

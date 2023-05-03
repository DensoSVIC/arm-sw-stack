..
 # SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 # affiliates <open-source-office@arm.com></text>
 #
 # SPDX-License-Identifier: MIT

#########################
Changelog & Release Notes
#########################

**********
Unreleased
**********

New Features
============

Changed
=======

Initial version.

.. _changelog_limitations:

Limitations
===========
 * In the HIPC, the iperf parameter "-l/--length" should be less than 1473 (IP
   and UDP overhead) in the case of Zephyr running as a UDP server since it does
   not support IP fragmentation.

Resolved and Known Issues
=========================

.. _changelog_knownissues:

Known Issues
------------
In the HIPC, some logs about packet buffer exhaustion or traffic sending
failure can be observed because of FVP performance bottleneck. Some logs and
possible causes are listed here:

 * In the zperf sample, when the packet buff pool is consumed,
   ``veth_rpmsg: Failed to allocate packet.`` will be printed.
 * FVP unfair scheduling results in asynchronism between the TCP sender and the
   TCP receiver. When the sender has sent many packets, and the receiver does
   not reply with any ack since it was not scheduled, it will cause the size of
   the TCP window to be too small to send any packet. Finally,
   ``Failed to send the packet (-11)`` will be logged.
 * In the zperf TCP testing case, if the application layer does not process the
   packet buffer in time after the connection is closed,
   ``net_tcp: context->tcp == NULL`` will be logged.

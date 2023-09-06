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

Implementation of the :ref:`Use-Cases <introduction_use_cases>`.

Components versions used in the Reference Stack:

.. list-table::
  :header-rows: 1

  * - Component
    - Version
    - Source
  * - Kronos Reference Design FVP (FVP_RD_Kronos)
    - |FVP_RD_Kronos version|
    - `FVP download`_
  * - Trusted Firmware-M (RSS)
    - |Trusted Firmware-M version| (based on |Trusted Firmware-M base version|)
    - `Trusted Firmware-M repository`_
  * - SCP-firmware
    - |SCP-Firmware version| (based on |SCP-Firmware base version|)
    - `SCP-Firmware repository`_
  * - Trusted Firmware-A
    - |Trusted Firmware-A version|
    - `Trusted Firmware-A repository`_
  * - OP-TEE
    - |OP-TEE version|
    - `OP-TEE repository`_
  * - Trusted Services
    - |Trusted Services version| (based on |Trusted Services base version|)
    - `Trusted Services repository`_
  * - U-Boot
    - |U-Boot version|
    - `U-Boot repository`_
  * - Xen
    - |Xen version|
    - `Xen repository`_
  * - Linux Kernel
    - |Linux version|
    - `Linux repository`_ and `Linux preempt-rt repository`_
  * - Zephyr
    - |Zephyr version|
    - `Zephyr repository`_
  * - Safety Island Actuation Demo
    - |Actuation version|
    - `Actuation repository`_
  * - Mbed TLS
    - |Mbed TLS version| (based on |Mbed TLS base version|)
    - `Mbed TLS repository`_

Third-party Yocto layers used to build the Reference Stack:

  .. code-block:: yaml
    :substitutions:

    URL: |meta-arm repository|
    layers: meta-arm, meta-arm-bsp, meta-arm-systemready, meta-arm-toolchain
    branch: |meta-arm branch|
    revision: |meta-arm revision|

    URL: |meta-cassini repository|
    layers: meta-cassini-distro
    branch: |meta-cassini branch|
    revision: |meta-cassini revision|

    URL: |meta-clang repository|
    layers: meta-clang
    branch: |meta-clang branch|
    revision: |meta-clang revision|

    URL: |meta-openembedded repository|
    layers: meta-filesystems, meta-networking, meta-oe, meta-python
    branch: |meta-openembedded branch|
    revision: |meta-openembedded revision|

    URL: |meta-security repository|
    layers: meta-parsec
    branch: |meta-security branch|
    revision: |meta-security revision|

    URL: |meta-virtualization repository|
    layers: meta-virtualization
    branch: |meta-virtualization branch|
    revision: |meta-virtualization revision|

    URL: |meta-zephyr repository|
    layers: meta-zephyr-core
    branch: |meta-zephyr branch|
    revision: |meta-zephyr revision|

    URL: |poky repository|
    layers: meta, meta-poky
    branch: |poky branch|
    revision: |poky revision|

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
 * The Zephyr FPU sharing functionality sometimes fails to restore the SIMD
   registers to their previous state, leading to the Actuation Demo Zephyr
   application to occasionally output wrong commands. A workaround is in place
   to fix this.

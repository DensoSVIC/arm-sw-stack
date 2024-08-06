..
 # SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
 # affiliates <open-source-office@arm.com></text>
 #
 # SPDX-License-Identifier: MIT

#############
Release Notes
#############

**********
Unreleased
**********

New Features
============

None.

Changed
=======

* Assemble the firmware images using genimage from the meta-ptx Yocto layer instead of wks/wic images
* Updated support from openSUSE 15.4 to 15.5
* Updated support from Debian 11.7 to 12.4
* Added Fedora 39.1.5 distribution to comply with the SystemReady IR v2.1 requirements.
* Added Fedora 39.1.5 distribution unattended installation option.
* Added compiler tuning for Cortex-R82 to the Zephyr toolchain
* Upgraded from Yocto nanbield to scarthgap
* Introduced the Yocto layer meta-arm-safety-island
* Removed LCP from the boot flow
* Aligned the number of supported MHUv3 channels with the RSE specification
* Enabled TF-A Trusted Board Boot (TBB)
* Enabled PSA Internal Trusted Storage API on Primary Compute
* Added an AP_REFCLK non-secure Generic Timer node in Kronos device tree
* Updated identified non-alignments on RD-Kronos for Devicetree missing schemas
* Introduced Safety Island GIC FMU device for Safety Island Cluster 1 and automated tests.
* Runtime Security Subsystem (RSE) has been renamed to Runtime Security Engine (RSE) since TF-M v2.1.0.

Third-party Yocto layers used to build the Reference Software Stack:

  .. code-block:: yaml
    :substitutions:

    URL: |meta-arm repository|
    layers: meta-arm, meta-arm-bsp, meta-arm-systemready, meta-arm-toolchain
    branch: scarthgap
    revision: 38bce82e42ea093333a53c4a10e51d1b26cbc989

    URL: |meta-cassini repository|
    layers: meta-cassini-distro, meta-cassini-tests
    branch: scarthgap
    tag: v2.0.0
    revision: bef1d728c6db464ff89828afae5b51e648058f35

    URL: |meta-clang repository|
    layers: meta-clang
    branch: scarthgap
    revision: 0acff283249842eb1f617b20c2ed4ebf9f8e3557

    URL: |meta-ewaol repository|
    layers: meta-ewaol
    branch: scarthgap
    tag: ewaol-2.0.0
    revision: c28142e72691202ba55a954f0faaed4375615b68

    URL: |meta-openembedded repository|
    layers: meta-filesystems, meta-networking, meta-oe, meta-python
    branch: scarthgap
    revision: 78a14731cf0cf38a19ff8bd0e9255b319afaf3a7

    URL: |meta-ptx repository|
    layers: meta-ptx
    branch: scarthgap
    revision: 547b079bf309ebe1576aa5ae0d58564feb245a42

    URL: |meta-security repository|
    layers: meta-parsec
    branch: scarthgap
    revision: 11ea91192d43d7c2b0b95a93aa63ca7e73e38034

    URL: |meta-virtualization repository|
    layers: meta-virtualization
    branch: scarthgap
    revision: 37c06acf58f9020bccfc61954eeefe160642d5f3

    URL: |meta-zephyr repository|
    layers: meta-zephyr-core
    branch: scarthgap
    revision: 763c72fc3088fc09ccfde6edfcdad43811d16616

    URL: |poky repository|
    layers: meta, meta-poky
    branch: scarthgap
    revision: ca27724b44031fe11b631ee50eb1e20f7a60009d

.. _releasenotes_limitations:

Limitations
===========

 * Same as `v1.0 Limitations`_ with the following exception:

   * Now, the Reference Software Stack also supports the Internal Trusted Storage (ITS)
     API on the Primary Compute.

Resolved and Known Issues
=========================

.. _releasenotes_knownissues:

Resolved Issues
---------------

  * Added runtime checks of Update Capsule flags in U-Boot, which fixed
    SystemReady IR ACS SCT Update Capsule test failure.

Known Issues
------------

  * For Heterogeneous Inter-Processor Communication (HIPC), during ping between
    Clusters, a transient issue is observed where ICMP replies take longer time
    to reach the originating Cluster.
  * Same as `v1.0 Known Issues`_.

****
v1.0
****

New Features
============

Implementation of the :ref:`Use-Cases <overview_use_cases>`.

The main components versions used in the Reference Software Stack:

..
  cspell:disable

.. list-table::
  :header-rows: 1

  * - Component
    - Version
    - Source
  * - Kronos Reference Design FVP (FVP_RD_Kronos)
    - 11.25.15
    - `FVP download (arm64 host) <https://developer.arm.com/-/media/Arm%20Developer%20Community/Downloads/OSS/FVP/Automotive%20FVPs/FVP_RD_Kronos_11.25_15_Linux64_armv8l.tgz>`_
      `FVP download (x86 host) <https://developer.arm.com/-/media/Arm%20Developer%20Community/Downloads/OSS/FVP/Automotive%20FVPs/FVP_RD_Kronos_11.25_15_Linux64.tgz>`_
  * - RSS (Trusted Firmware-M)
    - 53aa78efef274b9e46e63b429078ae1863609728 (based on master branch post v1.8.1)
    - `Trusted Firmware-M repository`_
  * - SCP-firmware
    - cc4c9e017348d92054f74026ee1beb081403c168 (based on master branch post v2.13.0)
    - `SCP-Firmware repository`_
  * - Trusted Firmware-A
    - 2.8.0
    - `Trusted Firmware-A repository`_
  * - OP-TEE
    - 3.22.0
    - `OP-TEE repository`_
  * - Trusted Services
    - 08b3d39471f4914186bd23793dc920e83b0e3197 (based on main branch, pre v1.0.0)
    - `Trusted Services repository`_
  * - U-Boot
    - 2023.07.02
    - `U-Boot repository`_
  * - Xen
    - 4.18
    - `Xen repository`_
  * - Linux Kernel
    - 6.1.73
    - `Linux repository`_ and `Linux preempt-rt repository`_
  * - Zephyr
    - 3.5.0
    - `Zephyr repository`_
  * - Safety Island Actuation Demo
    - v2.0
    - `Actuation repository`_
  * - Mbed TLS
    - 1ec69067fa1351427f904362c1221b31538c8b57 (based on 3.5.0)
    - `Mbed TLS repository`_
  * - Critical Application Monitoring
    - v1.0
    - `Critical Application Monitoring repository`_

..
  cspell:enable

Third-party Yocto layers used to build the Reference Software Stack:

  .. code-block:: yaml
    :substitutions:

    URL: |meta-arm repository|
    layers: meta-arm, meta-arm-bsp, meta-arm-systemready, meta-arm-toolchain
    branch: kronos-nanbield
    revision: 5e4851a884985b952b33f6f88a8724fbbe5300ec

    URL: |meta-cassini repository|
    layers: meta-cassini-distro
    branch: nanbield
    revision: v1.1.0

    URL: |meta-clang repository|
    layers: meta-clang
    branch: nanbield
    revision: 5170ec9cdfe215fcef146fa9142521bfad1d7d6c

    URL: |meta-openembedded repository|
    layers: meta-filesystems, meta-networking, meta-oe, meta-python
    branch: nanbield
    revision: da9063bdfbe130f424ba487f167da68e0ce90e7d

    URL: |meta-security repository|
    layers: meta-parsec
    branch: nanbield
    revision: 5938fa58396968cc6412b398d403e37da5b27fce

    URL: |meta-virtualization repository|
    layers: meta-virtualization
    branch: nanbield
    revision: ac125d881f34ff356390e19e02964f8980d4ec38

    URL: |meta-zephyr repository|
    layers: meta-zephyr-core
    branch: nanbield
    revision: fa76b75bd65da63abcc2d65dd5d4eb24296f2f65

    URL: |poky repository|
    layers: meta, meta-poky
    branch: nanbield
    revision: 1a5c00f00c14cee3ba5d39c8c8db7a9738469eab

Changed
=======

Initial version.

.. _v1.0 Limitations:

Limitations
===========

 * In the HIPC, the iperf parameter "-l/--length" should be less than 1473 (IP
   and UDP overhead) in the case of Zephyr running as a UDP server since it does
   not support IP fragmentation.
 * `PSA Secure Storage API`_ defines two interfaces for storages: Internal
   Trusted Storage (ITS) API and Protected Storage (PS) API. For now the
   Reference Software Stack supports the ITS API on Safety Island only.
 * PSA Protected Storage Optional APIs ``psa_ps_create`` and ``psa_ps_extended``
   are not supported by Arm Automotive Solutions as they are not implemented in
   the Protected Storage Service provided by Trusted Firmware-M.
 * PSA Secure Storage APIs Architecture Test Suite only runs on
   Cluster 2 in the Safety Island due to the following limitations:

   * Trusted Firmware-M supports a single partition only, this causes
     tests running simultaneously on different entities to interfere
     with each other due to accessing the same assets, resulting in failures.
   * Trusted Firmware-M has no support against Denial of Service attacks,
     where a test running on one entity might take up all the storage
     on the RSS resulting in denial of service for tests running on other
     entities.

Resolved and Known Issues
=========================

.. _v1.0 Known Issues:

Known Issues
------------
  * The automated validation might fail due to the encoding issues in the logs.
    This has been observed on an AWS aarch64 Graviton 2 build host. On the test logs,
    the error message that appears is a typical timeout error.

    The console log appears normal, but some characters are either corrupted or
    replaced with \00, \x00 or ^@ characters. This issue is likely caused by encoding
    mismatches or inconsistencies in the logging process, and it could occur in any of
    the test suites. A workaround is to trigger the "Automated Validation" again. When
    this issue occurs, something similar to the following would be observed in the logs:

    .. cspell:disable

    .. code-block:: text

      52 28 bytes from 192.168.1.2 to 192.168.1.1: icmp_seq=7 ttl=64 time=0.00 ^@s^M
      or
      fault set_critical f\00u@2a570000 0x10000600 0
      or
      System shutdown complet\x00

    .. cspell:enable

  * The automated validation might rarely fail due to timeouts related to the
    host CPU frequency and throttling, if this happens then simply running the
    automated validation again would fix such as issue.

  * Refer to `Critical Application Monitoring Known Issues <https://critical-app-monitoring.docs.arm.com/en/v1.0/release_notes.html#known-issues>`_
    for CAM-related known issues.

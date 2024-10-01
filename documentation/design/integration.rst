..
 # SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
 # affiliates <open-source-office@arm.com></text>
 #
 # SPDX-License-Identifier: MIT

###########
Integration
###########

The Reference Software Stack uses the Yocto Project build framework to build,
integrate and validate the :ref:`Use-Cases <overview_use_cases>`.

The Yocto Project version used by the Reference Software Stack
is |yocto version|.

*******************************
meta-arm-bsp-extras Yocto Layer
*******************************

The ``meta-arm-bsp-extras`` layer primarily depends on the ``meta-arm`` layer
which implements the ``fvp-rd-kronos`` bitbake ``MACHINE`` definition to enable
the Arm Automotive Solutions to run on the Arm Reference Design-1 AE FVP
(FVP_RD_1_AE).

***********************************
meta-arm-auto-solutions Yocto Layer
***********************************

The ``meta-arm-auto-solutions`` layer is based on the `EWAOL`_ distribution.
It also contains a set of bitbake bbclasses, recipes and libraries to build,
integrate, and validate the :ref:`overview_use_cases` with either or both the
**Baremetal** and **Virtualization** Reference Software Stack Architectures
as described in :ref:`Reference Software Stack Overview
<overview_reference_software_stack_overview>`.

It depends on a BSP layer such as the ``meta-arm-bsp-extras`` layer.

The layer source code can be found at :repo:`yocto/meta-arm-auto-solutions`.

**********************************
meta-arm-safety-island Yocto Layer
**********************************

The layer ``meta-arm-safety-island`` integrates the applications for the Safety
Island using base configuration from the ``meta-zephyr-core`` Yocto layer. It
also contains recipes for the Linux kernel modules for inter-processor
communication with the Safety Island.

The layer source code can be found at :repo:`yocto/meta-arm-safety-island`.

Yocto Build Configuration
=========================

A set of ``yaml`` configuration files (found at :repo:`yocto/kas`) for
the `kas build tool`_ is provided to support bitbake layer fetching, project
configuration and executing the build and validation.

Yocto Layers Dependency
=======================

The following diagram illustrates the layers which are integrated as part of
the Reference Software Stack.

|

.. image:: ../images/yocto_layers_dependency_diagram.*
   :align: center
   :alt: Yocto Layer Dependency

|

**Note** that the ``meta-arm-systemready`` layer is only required when building
for the Arm SystemReady IR ACS tests.

The layer dependency sources and their revisions for the ``kronos`` repository
(|arm auto solutions repository|) |layer dependency statement| are:

  .. code-block:: yaml
    :substitutions:

    URL: |meta-arm repository|
    layers: meta-arm, meta-arm-bsp, meta-arm-systemready, meta-arm-toolchain
    branch: |meta-arm branch|
    revision: |meta-arm revision|

    URL: |meta-cassini repository|
    layers: meta-cassini-distro, meta-cassini-tests
    branch: |meta-cassini branch|
    revision: |meta-cassini revision|

    URL: |meta-clang repository|
    layers: meta-clang
    branch: |meta-clang branch|
    revision: |meta-clang revision|

    URL: |meta-ewaol repository|
    layers: meta-ewaol
    branch: |meta-ewaol branch|
    revision: |meta-ewaol revision|

    URL: |meta-openembedded repository|
    layers: meta-filesystems, meta-networking, meta-oe, meta-python, meta-perl
    branch: |meta-openembedded branch|
    revision: |meta-openembedded revision|

    URL: |meta-ptx repository|
    layers: meta-ptx
    branch: |meta-ptx branch|
    revision: |meta-ptx revision|

    URL: |meta-secure-core repository|
    layers: meta-secure-core-common, meta-efi-secure-boot, meta-signing-key
    branch: |meta-secure-core branch|
    revision: |meta-secure-core revision|

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

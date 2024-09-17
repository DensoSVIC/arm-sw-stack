..
 # SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
 # affiliates <open-source-office@arm.com></text>
 #
 # SPDX-License-Identifier: MIT

#########
Customize
#########

.. warning::
  The use of custom build configuration is not validated.

*************************************
Available build environment variables
*************************************

The available build configuration options and the associated environment
variables are as follows:

.. warning::
  For the Kronos FVP, the amount of RAM cannot exceed 4064MB. This is derived
  by taking the 4096MB total and minusing 32MB for OP-TEE. For the
  Virtualization architecture, this must be split between Xen, Dom0, DomU1 and
  DomU2.

.. list-table::
  :header-rows: 1

  * - Property
    - Architecture
    - Variable
    - Default
  * - Primary Compute RAM size
    - Baremetal
    - ``BAREMETAL_IMAGE_MEM_SIZE``
    - ``2016M``
  * - Dom0 RAM size
    - Virtualization
    - ``DOM0_MEMORY_SIZE``
    - ``1024M``
  * - DomU1 RAM size
    - Virtualization
    - ``DOMU1_MEMORY_SIZE``
    - ``1024`` (fixed units of MB)
  * - DomU2 RAM size
    - Virtualization
    - ``DOMU2_MEMORY_SIZE``
    - ``1024`` (fixed units of MB)
  * - Primary Compute rootfs partition extra size
    - Baremetal and Virtualization (Dom0 only)
    - ``CASSINI_ROOTFS_EXTRA_SPACE``
    - ``0`` (fixed units of KB)

*********************************
Using build environment variables
*********************************

The Reference Software Stack image offers configuration via environment
variables. In order to make use of these variables, when reproducing one of the
:ref:`Use-Cases <overview_use_cases>` using the
:ref:`reproduce instructions<user_guide_reproduce>`, follow these steps:

1. Export the relevant build environment variables.

   As an example for the Baremetal architecture, to set the Primary Compute RAM
   size to 4064MB and rootfs partition extra size to 5GB, issue:

   .. code::

     export BAREMETAL_IMAGE_MEM_SIZE=4064M
     export CASSINI_ROOTFS_EXTRA_SPACE=5242880

   As an example for the Virtualization architecture, to set the Primary
   Compute Dom0 RAM size to 992MB, the DomU1 RAM size to 1536MB, keep the
   DomU2 memory size at the 1024MB default and set the Dom0 rootfs partition
   extra size to 5GB, issue:

   .. code::

     export DOMU_MEMORY_SIZE=992M
     export DOMU1_MEMORY_SIZE=1536
     export DOMU2_MEMORY_SIZE=1024
     export CASSINI_ROOTFS_EXTRA_SPACE=5242880

2. From the ``kas menu``, choose the appropriate options then choose ``Build``.
3. Remember to ``unset`` the environment variables for future builds not to
   use them.

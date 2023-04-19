#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: Apache-2.0

zephyr_library()
zephyr_library_sources_ifdef(CONFIG_VETH_RPMSG veth_rpmsg.c)

if(CONFIG_VETH_RPMSG)
   # METAL_MAX_DEVICE_REGIONS is used to give the number of memory regions shared
   # between processors. By default only one region is defined for the vrings
   # and rpmsg buffers. The METAL_MAX_DEVICE_REGIONS has to be redefined to add a
   # second region for the resource table.
   zephyr_compile_definitions(METAL_MAX_DEVICE_REGIONS=2)
endif()

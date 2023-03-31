#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# Additional configuration for the fvp-rd-kronos with the "xen"
# DISTRO_FEATURE

# Xen is booted using the "chainloader" command provided by the "chain" module
GRUB_BUILDIN:append = " chain"

# Remove SDL from Xen's PACKAGECONFIG, which adds many dependencies that are
# not needed
PACKAGECONFIG:remove:pn-xen = "sdl"
PACKAGECONFIG:remove:pn-xen-tools = "sdl"

# Remove unneeded items from qemu PACKAGECONFIG
PACKAGECONFIG:remove:pn-qemu = "sdl alsa virglrenderer epoxy kvm pie slirp"

# Xen uses console hvc0
SERIAL_CONSOLES = "115200;hvc0"

# Prepend the local wic directory to the wks search path
WKS_SEARCH_PATH:prepend := "${KRONOS_VIRTUALIZATION_DYNAMIC_DIR}/wic:"

# Set WKS_FILE for Xen. Xen will use ${GRUB_CFG_FILE} as configfile
WKS_FILE = "fvp-rd-kronos-xen-efi-disk.wks.in"

# Set up the tests
TEST_FVP_LINUX_BOOT_TIMEOUT="${@40*60}"

# Skip BSP tests. It will be done in the future
TEST_SUITES:remove="test_20_bsp"

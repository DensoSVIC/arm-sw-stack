#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

PREFERRED_PROVIDER_virtual/kernel = "linux-yocto"

# Additional configuration for the fvp-rd-kronos with the "virtualization"
# IMAGE_FEATURE

# Xen is booted using the "chainloader" command provided by the "chain" module
GRUB_BUILDIN:append = " chain"

# Remove SDL from Xen's PACKAGECONFIG, which adds many dependencies that are
# not needed
PACKAGECONFIG:remove:pn-xen = "sdl"
PACKAGECONFIG:remove:pn-xen-tools = "sdl"

# Remove unneeded items from qemu PACKAGECONFIG
PACKAGECONFIG:remove:pn-qemu = "sdl alsa virglrenderer epoxy kvm slirp"

# Xen uses console hvc0
SERIAL_CONSOLES = "115200;hvc0"

# Set WKS_FILE for Xen. Xen will use ${GRUB_CFG_FILE} as configfile
WKS_FILE = "fvp-rd-kronos-xen-efi-disk.wks.in"

# Provide Passthrough PCI AHCI SATA disk with an empty image
PCI_IMAGE_NAME = "xen-pci-test-image"
DOMU1_PCI_ID ?= "0000:00:1f.0"
FVP_CONFIG[pcie_group_0.pcie4.pcie_rc.ahci0.ahci.image_path] = "${DEPLOY_DIR_IMAGE}/${PCI_IMAGE_NAME}.wic"
EXTRA_IMAGEDEPENDS += "${PCI_IMAGE_NAME}:do_deploy"

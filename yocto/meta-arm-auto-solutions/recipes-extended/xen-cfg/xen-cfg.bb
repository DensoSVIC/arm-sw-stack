# nooelint: oelint.var.mandatoryvar - There is no source file
# The "source" is in the do_deploy task.
#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "Xen EFI config file"
DESCRIPTION = "Deploy a .cfg file that can be used to boot Xen using EFI"
HOMEPAGE = "https://wiki.xenproject.org/wiki/Xen_EFI"
LICENSE = "MIT"

# License file is in "layers/poky/meta/files/common-licenses".
# nooelint: oelint.var.licenseremotefile
LIC_FILES_CHKSUM = "\
    file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302 \
    "

inherit deploy features_check
REQUIRED_IMAGE_FEATURES += "virtualization"

do_patch[noexec] = "1"
do_configure[noexec] = "1"
do_compile[noexec] = "1"
do_install[noexec] = "1"

# Set Dom0 VCPU affinity, MPAM SLC and SVE2 config
DOM0_SVE_SETTING ?= "${@ '128' if d.getVar('SVE_DISABLE_FLAG', True) != '1' else '0'}"
EXTRA_XEN_CMDLINE_CONFIG ?= "maxcpus=4 dom0_max_vcpus=1 dom0_vcpus_pin dom0_mpam=slc:0xf iommu=yes dom0=sve=${DOM0_SVE_SETTING}"

# Set Dom0 Static passthrough PCI AHCI SATA disk assignment
EXTRA_PCI_PASSTHROUGH_CONFIG ?= "xen-pciback.hide=(${DOMU1_PCI_ID})"

do_deploy() {
    cat << EOF > ${WORKDIR}/xen.cfg
[global]
default=xen

[xen]
options=noreboot dom0_mem=1024M ${EXTRA_XEN_CMDLINE_CONFIG}
kernel=Image console=hvc0 earlycon=xenboot root=/dev/vda2 rootwait ${EXTRA_PCI_PASSTHROUGH_CONFIG}
EOF
    cp ${WORKDIR}/xen.cfg ${DEPLOYDIR}/xen.cfg
}
addtask deploy after do_install before do_build

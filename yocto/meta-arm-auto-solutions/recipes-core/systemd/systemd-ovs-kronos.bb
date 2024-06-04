#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "Systemd Open vSwitch bridges Kronos configuration"
DESCRIPTION = "Systemd service to setup the network switches for Kronos"
HOMEPAGE = "http://www.freedesktop.org/wiki/Software/systemd"
LICENSE = "MIT"
# License file is in "layers/poky/meta/files/common-licenses".
# nooelint: oelint.var.licenseremotefile
LIC_FILES_CHKSUM = "\
    file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302 \
    "

SRC_URI = "\
    file://ovs-kronos-ovsbr0.service;subdir=src \
    file://kronos-ovs.sh;subdir=src \
    "

SRC_URI:append:virtualization = " \
    file://xendomains.conf;subdir=src \
    "

S = "${WORKDIR}/src"

inherit features_check systemd

REQUIRED_DISTRO_FEATURES = "systemd"

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE:${PN} = "ovs-kronos-ovsbr0.service"

do_install() {
    install -d ${D}/${systemd_unitdir}/system/
    install -m 644 ${S}/ovs-kronos-ovsbr0.service \
        ${D}/${systemd_unitdir}/system/ovs-kronos-ovsbr0.service
    install -d ${D}${sbindir}
    install -m 0755 ${S}/kronos-ovs.sh ${D}${sbindir}/kronos-ovs.sh
}

do_install:append:virtualization() {
    install -d ${D}${sysconfdir}/systemd/system/xendomains.service.d/
    install -m 644 ${S}/xendomains.conf \
        ${D}${sysconfdir}/systemd/system/xendomains.service.d/xendomains.conf
}

RECIPE_FILES = "\
    ${systemd_unitdir}/system/ovs-kronos-ovsbr0.service \
    ${sbindir}/kronos-ovs.sh \
    "

RECIPE_FILES:append:virtualization = " \
    ${sysconfdir}/systemd/system/xendomains.service.d/xendomains.conf \
    "

FILES:${PN} += "${RECIPE_FILES}"
RDEPENDS:${PN} = "systemd"

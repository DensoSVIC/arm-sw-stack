#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = " file://ptp4l-override.conf"
SRC_URI:append:baremetal = " file://ptp4l.conf"
SRC_URI:append:virtualization = " file://ptp4l.conf"
SRC_URI:append:domu = " file://ptp4l-domu.conf"

inherit features_check

# This variable is computed dynamically by the features_check bbclass
# nooelint: oelint.vars.mispell
ANY_OF_IMAGE_FEATURES = "baremetal virtualization domu"

LINUXPTP_SYSTEMD_SERVICES = "ptp4l@.service"

CFG_FILE = "ptp4l.conf"
CFG_FILE:domu = "ptp4l-domu.conf"

do_install:append() {
    # Update default config file for ptp4l
    install -m 644 ${WORKDIR}/${CFG_FILE} \
        ${D}${sysconfdir}/linuxptp/ptp4l.conf

    # Enable the service(s)
    install -d ${D}${sysconfdir}/systemd/system/multi-user.target.wants/
    for iface in ${LINUXPTP_IFACES}; do
        ln -sf ${systemd_unitdir}/system/ptp4l@.service \
            ${D}${sysconfdir}/systemd/system/multi-user.target.wants/ptp4l@${iface}.service
    done

    # Install the ptp4l systemd service drop-in file
    install -d ${D}${sysconfdir}/systemd/system/ptp4l@.service.d/
    install -m 644 ${WORKDIR}/ptp4l-override.conf \
        ${D}${sysconfdir}/systemd/system/ptp4l@.service.d/ptp4l-override.conf
}

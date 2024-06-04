#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend := "${THISDIR}/kronos-files:"

KRONOS_LINUXPTP_SRC_URI_EXTRA = "file://ptp4l-override.conf"
KRONOS_LINUXPTP_SRC_URI_EXTRA:append:baremetal = " file://ptp4l.conf"
KRONOS_LINUXPTP_SRC_URI_EXTRA:append:virtualization = " file://ptp4l.conf"
KRONOS_LINUXPTP_SRC_URI_EXTRA:append:domu = " file://ptp4l-domu.conf"

SRC_URI:append = " ${KRONOS_LINUXPTP_SRC_URI_EXTRA}"

inherit features_check

# This variable is computed dynamically by the features_check bbclass
# nooelint: oelint.vars.mispell
ANY_OF_IMAGE_FEATURES = "baremetal virtualization domu"

LINUXPTP_SYSTEMD_SERVICES = "ptp4l@.service"

KRONOS_PTP4L_CFG_FILE = "ptp4l.conf"
KRONOS_PTP4L_CFG_FILE:domu = "ptp4l-domu.conf"

do_install:append() {
    # Update default config file for ptp4l
    install -m 644 ${WORKDIR}/${KRONOS_PTP4L_CFG_FILE} \
        ${D}${sysconfdir}/linuxptp/ptp4l.conf

    # Enable the service(s)
    install -d ${D}${sysconfdir}/systemd/system/multi-user.target.wants/
    for iface in ${LINUXPTP_IFACES}; do
        ln -sf ${systemd_unitdir}/system/ptp4l@.service \
            ${D}${sysconfdir}/systemd/system/multi-user.target.wants/ptp4l@${iface}.service
    done

    # Install the kronos ptp4l systemd service drop-in file
    install -d ${D}${sysconfdir}/systemd/system/ptp4l@.service.d/
    install -m 644 ${WORKDIR}/ptp4l-override.conf \
        ${D}${sysconfdir}/systemd/system/ptp4l@.service.d/ptp4l-override.conf
}

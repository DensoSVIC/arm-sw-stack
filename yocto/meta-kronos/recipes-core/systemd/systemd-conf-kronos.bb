# nooelint: oelint.var.mandatoryvar - This recipe has no source files
#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "Systemd configuration for Kronos"
DESCRIPTION = "Additional systemd configuration for the Kronos stacks"
HOMEPAGE = "http://www.freedesktop.org/wiki/Software/systemd"
LICENSE = "MIT"

PACKAGE_WRITE_DEPS += "systemd-systemctl-native"

S = "${WORKDIR}"

inherit features_check

INHIBIT_DEFAULT_DEPS = "1"

ALLOW_EMPTY:${PN} = "1"

REQUIRED_DISTRO_FEATURES = "systemd"

KRONOS_SYSTEMD_DISABLE_SERVICES = "\
    systemd-timesyncd.service \
    systemd-tmpfiles-clean.timer \
"

pkg_postinst:${PN} () {
    if [ -n "$D" ]; then
        OPTS="--root=$D"
    fi
    for service in ${KRONOS_SYSTEMD_DISABLE_SERVICES}; do
        systemctl $OPTS mask $service
    done
}

RDEPENDS:${PN} = "systemd"

# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "Systemd configuration for Arm Automotive Solutions"
DESCRIPTION = "Additional systemd configuration for the Arm Automotive Solutions stacks"
HOMEPAGE = "http://www.freedesktop.org/wiki/Software/systemd"
LICENSE = "MIT"

PACKAGE_WRITE_DEPS += "systemd-systemctl-native"

S = "${WORKDIR}"

inherit features_check

INHIBIT_DEFAULT_DEPS = "1"

ALLOW_EMPTY:${PN} = "1"

REQUIRED_DISTRO_FEATURES = "systemd"

SYSTEMD_DISABLE_SERVICES = "\
    systemd-timesyncd.service \
    systemd-tmpfiles-clean.timer \
"

pkg_postinst:${PN} () {
    if [ -n "$D" ]; then
        OPTS="--root=$D"
    fi
    for service in ${SYSTEMD_DISABLE_SERVICES}; do
        systemctl $OPTS mask $service
    done
}

RDEPENDS:${PN} = "systemd"

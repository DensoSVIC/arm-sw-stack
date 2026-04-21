DESCRIPTION = "Telegraf"
SUMMARY = "Telegraf is the open source server agent to help you collect metrics from your stacks, sensors and systems."
HOMEPAGE = "https://www.influxdata.com/time-series-platform/telegraf/"

INSANE_SKIP:${PN} += "already-stripped"

PR = "r1"

LICENSE = "CLOSED"

SRC_URI = "https://dl.influxdata.com/telegraf/releases/telegraf-1.30.0_linux_arm64.tar.gz"
SRC_URI[sha256sum] = "6c82fc5ec6b72d75074165263e7dc31114e83cfbc002617052c6c5983d8fa9e4"

# FIX APPLIED HERE
S = "${WORKDIR}/telegraf-${PV}"

do_install() {
    # /etc
    install -d ${D}${sysconfdir}/logrotate.d
    install -d ${D}${sysconfdir}/telegraf
    install -d ${D}${sysconfdir}/telegraf/telegraf.d

    install -m 0644 ${S}/etc/logrotate.d/telegraf ${D}${sysconfdir}/logrotate.d/
    install -m 0644 ${S}/etc/telegraf/telegraf.conf ${D}${sysconfdir}/telegraf/

    # /usr/bin
    install -d ${D}${bindir}
    install -m 0755 ${S}/usr/bin/telegraf ${D}${bindir}/

    # /usr/lib (systemd service)
    install -d ${D}${systemd_unitdir}/system
    sed -i 's/User=telegraf/User=root/g' ${S}/usr/lib/telegraf/scripts/telegraf.service
    install -m 0644 ${S}/usr/lib/telegraf/scripts/telegraf.service ${D}${systemd_unitdir}/system

    # /var
    install -d ${D}${localstatedir}/log/telegraf
}

inherit systemd
SYSTEMD_SERVICE:${PN} = "telegraf.service"

# Prevent unpackaged empty directory errors for /var/log/telegraf
FILES:${PN} += "${localstatedir}/log/telegraf"
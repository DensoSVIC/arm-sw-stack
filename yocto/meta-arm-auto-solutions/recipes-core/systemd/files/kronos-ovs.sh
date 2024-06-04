#! /bin/sh

#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

set -e

start() {
    ovs-vsctl --may-exist add-br ovsbr0
    ovs-vsctl --may-exist add-port ovsbr0 eth0
    ovs-vsctl --may-exist add-br brsi0 ovsbr0 100
    ovs-vsctl --may-exist add-br brsi1 ovsbr0 200
    ovs-vsctl --may-exist add-br brsi2 ovsbr0 300
    ovs-vsctl --may-exist add-port brsi0 ethsi0 vlan_mode=native-tagged
    ovs-vsctl --may-exist add-port brsi1 ethsi1 vlan_mode=native-tagged
    ovs-vsctl --may-exist add-port brsi2 ethsi2 vlan_mode=native-tagged
}

stop() {
    ovs-vsctl --if-exist del-br ovsbr0
}

reload() {
    stop
    start
}

case "$@" in
    start)
        start
        ;;
    stop)
        stop
        ;;
    reload)
        reload
        ;;
    *)
        exit 1
        ;;
esac

exit 0

#!/usr/bin/env bats
#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# bats file_tags=xendomains,virtualization,dom0

check_domu_status() {
    domus=$(grep -h '^name' /etc/xen/auto/*.cfg | \
            sed 's/.*name\s*=\s*"\(.*\)".*/\1/')
    for domu in ${domus}; do
        run bash -c "xl list | grep ${domu}"
        echo "${output}"
        [ "$status" -eq "$1" ]
    done
}

@test "Check restarting Xen domains" {
    check_domu_status 0

    echo "# Stopping Xen domains" >&3
    # Both Xen and BATS use file descriptor 3, so it is necessary to duplicate
    # the file handle
    run /etc/init.d/xendomains stop 3>&-
    echo "${output}"
    [ "$status" -eq 0 ]

    check_domu_status 1

    echo "# Restarting Xen domains" >&3
    run /etc/init.d/xendomains start 3>&-
    echo "${output}"
    [ "$status" -eq 0 ]

    check_domu_status 0
}

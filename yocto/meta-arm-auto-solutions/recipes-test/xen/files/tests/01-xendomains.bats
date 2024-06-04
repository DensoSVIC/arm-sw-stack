#!/usr/bin/env bats
#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
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

check_cpbm_value() {
    local dom
    local value
    dom=$1
    value=$2

    # The output format of `xl psr-cat-show` would be:
    # <ID> <Domain Name> <CPBM>
    run bash -c "xl psr-cat-show -l 0 $dom | grep $dom | awk '{print \$3}'"
    echo "${output}"
    [ "$status" -eq 0 ]
    [ $((output)) -eq $((value)) ]
}

check_domu_cpbm_values() {
    local index=0
    # We only support maximum 2 DomUs, so a two-element array is enough
    local domain_cpbm_values_0=("0xc0" "0x30")
    local domain_cpbm_values_1=("0x10" "0x100")
    local domain_cpbm_values_2=("0xf0" "0xf00")

    domus=$(grep -h '^name' /etc/xen/auto/*.cfg | \
            sed 's/.*name\s*=\s*"\(.*\)".*/\1/')

    for domu in ${domus}; do
        # DomUs have been set with initial CPBM (cache portion bitmask) at boot
        # Verify if the value is consistent with the configured value
        # Initial CPBM: DomU1=0xc0, DomU2=0x30
        check_cpbm_value ${domu} ${domain_cpbm_values_0[index]}

        # Apply MPAM config for DomU and verify
        run xl psr-cat-set -l 0 ${domu} ${domain_cpbm_values_1[index]}
        [ "$status" -eq 0 ]
        check_cpbm_value ${domu} ${domain_cpbm_values_1[index]}

        # Apply a different MPAM config for DomU and verify
        run xl psr-cat-set -l 0 ${domu} ${domain_cpbm_values_2[index]}
        [ "$status" -eq 0 ]
        check_cpbm_value ${domu} ${domain_cpbm_values_2[index]}

        index=$((index + 1))
    done
}

check_gicv4.1_functionality() {
    local cpu_pool=("CPU0" "CPU1" "CPU2" "CPU3")
    local its_pool=("0000000030040000" "0000000030080000" "00000000300c0000"
                    "0000000030100000" "0000000030140000" "0000000030180000")

    # In Kronos, we have 4 PCPUS
    for cpu in ${cpu_pool}; do
        # Xen Redistributor boot log shall have the following print
        # indicating the hardware supports GICV4.1 virtual LPI direct injection
        # "(XEN) GICv4: CPUx: VLPI support, ..., RVPEID support"
        run bash -c "xl dmesg | grep \"$cpu: VLPI support.*RVPEID support\""
        [ "$status" -eq 0 ]
    done

    # In Kronos, we have 6 v4.1 ITSes
    for its in ${its_pool}; do
        # Xen ITS boot log shall have the following print indicating
        # the hardware has v4.1 ITS
        # "(XEN) ITS@0000000030040000: Using GICv4.1 mode 00000000 00000001
        run bash -c "xl dmesg | grep \"ITS@$its: Using GICv4.1 mode\""
        [ "$status" -eq 0 ]
    done

    # SGI in DOM0 shall be the new, HW-based ones which do not have active state
    # DOM0 log shall contain the following output:
    # "Enabling SGIs without active state"
    run bash -c "dmesg | grep \"Enabling SGIs without active state\""
    [ "$status" -eq 0 ]
}

@test "Check restarting Xen domains" {
    # Ensure xendomains has started
    systemctl is-system-running --wait

    check_domu_status 0

    echo "# Stopping Xen domains"
    systemctl stop xendomains
    echo "${output}"
    [ "$status" -eq 0 ]

    check_domu_status 1

    echo "# Restarting Xen domains"
    systemctl start xendomains

    check_domu_status 0
}

@test "Verify MPAM enablement" {
    # Dom0 has been set with 0xf as the CPBM (cache portion bitmask) at boot
    # Verify if the value is consistent with the configured value
    check_cpbm_value Domain-0 0xf

    check_domu_cpbm_values
}

@test "Verify GICv4.1 feature enablement" {
    # GICv4.1 is an extension to GICv3, with new features of direct vLPI
    # and vSGI injection
    check_gicv4.1_functionality
}

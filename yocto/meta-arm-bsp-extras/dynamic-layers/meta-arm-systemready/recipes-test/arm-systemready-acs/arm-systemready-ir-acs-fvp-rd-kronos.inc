#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:append := "${THISDIR}/files/${MACHINE}:"
SRC_URI:append = " file://report.txt"

# Use SystemReady IR ACS image 2.1.0
PV = "2.1.0"
PV_DATE = "23.09"
SRC_URI[acs-img.sha256sum] = "c9bf2abfe06878f4c47af5052a73260239f61860e3bae39345874b4fae6b3843"

# The do_deploy task depends on BUILD_ARCH, which is excluded from the task signature
do_deploy[nostamp] = "1"

# Ensure FVP shuts down cleanly
TEST_SUITES += "arm_systemready_ir_acs_shutdown"

# Monkey-patch the acs_logs_handle function to handle the change in partition number for v2.1.0
python() {
    acs_logs_handle = d.getVar("acs_logs_handle")
    acs_logs_handle = acs_logs_handle.replace("':3/acs_results'", "':1/acs_results'")
    d.setVar("acs_logs_handle", acs_logs_handle)
}

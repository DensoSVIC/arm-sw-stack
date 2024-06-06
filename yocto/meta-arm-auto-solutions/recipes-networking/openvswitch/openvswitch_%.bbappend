#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# nooelint: oelint.vars.insaneskip
INSANE_SKIP:${PN}-ptest:baremetal += "buildpaths"
# nooelint: oelint.vars.insaneskip
INSANE_SKIP:${PN}-ptest:virtualization += "buildpaths"

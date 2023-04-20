#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.runtime.case import OERuntimeTestCase


class SafetyIslandC2Test(OERuntimeTestCase):
    console_cluster2 = 'safety_island_c2'

    def smp_boot(self, console):
        self.target.expect(console,
                           r'Secondary CPU core 1 \(MPID:(0x[0-9]+)\) is up',
                           timeout=120)
        self.target.expect(console,
                           r'Secondary CPU core 2 \(MPID:(0x[0-9]+)\) is up',
                           timeout=120)
        self.target.expect(console,
                           r'Secondary CPU core 3 \(MPID:(0x[0-9]+)\) is up',
                           timeout=120)

    def test_cluster2(self):
        self.smp_boot(self.console_cluster2)

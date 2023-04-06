#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.runtime.case import OERuntimeTestCase
from oeqa.runtime.cases.test_10_safety_island_base import SafetyIslandTestBase


class SafetyIslandC2Test(SafetyIslandTestBase):
    console_cluster2 = 'safety_island_c2'

    def smp_boot(self, console):
        self.target.expect(console,
                           r'Secondary CPU core 1 \(MPID:0x0\) is up',
                           timeout=120)
        self.target.expect(console,
                           r'Secondary CPU core 2 \(MPID:0x100\) is up',
                           timeout=120)
        self.target.expect(console,
                           r'Secondary CPU core 3 \(MPID:0x200\) is up',
                           timeout=120)

    def test_cluster2(self):
        self.smp_boot(self.console_cluster2)
        self.synchronization_sample(self.console_cluster2)

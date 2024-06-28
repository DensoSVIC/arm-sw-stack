#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.runtime.case import OERuntimeTestCase


class SafetyIslandC1Test(OERuntimeTestCase):
    console_cluster1 = 'safety_island_c1'

    def smp_boot(self, console):
        self.target.transition('on')
        self.target.expect(console,
                           r'Secondary CPU core 1 \(MPID:(0x[0-9]+)\) is up',
                           timeout=120)

    def test_cluster1(self):
        self.smp_boot(self.console_cluster1)

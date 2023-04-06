#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.runtime.case import OERuntimeTestCase


class SafetyIslandTestBase(OERuntimeTestCase):

    def synchronization_sample(self, console):
        self.target.expect(console,
                           r'thread_a: Hello World from cpu 0 on '
                           'fvp_rd_kronos_cortex_r82!',
                           timeout=120)
        self.target.expect(console,
                           r'thread_b: Hello World from cpu 1 on '
                           'fvp_rd_kronos_cortex_r82!',
                           timeout=120)

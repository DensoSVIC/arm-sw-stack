#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.runtime.case import OERuntimeTestCase


class SafetyIslandC0Test(OERuntimeTestCase):
    console = 'safety_island_c0'

    def test_cluster0(self):
        self.target.transition('on')
        self.target.expect(self.console,
                           r'Hello World! [\w_]+_safety_island',
                           timeout=120)

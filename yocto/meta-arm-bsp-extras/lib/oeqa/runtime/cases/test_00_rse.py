#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.runtime.case import OERuntimeTestCase


class RseTest(OERuntimeTestCase):
    console = 'rse'

    def test_normal_boot(self):
        self.target.transition('on')
        if self.td.get('MACHINE', '') == "fvp-rd-kronos":
            result = self.target.expect(self.console,
                                        r'Multiple Views configuration done',
                                        timeout=30)
            self.assertEqual(result, 0, "Failed to program GIC-Multiview")
            result = self.target.expect(
                         self.console,
                         r'Safety Island runtime APU programming done',
                         timeout=30)
            self.assertEqual(result, 0, "Failed to program NI-710AE")

        self.target.expect(self.console,
                           r'Jumping to the first image slot',
                           timeout=30)
        self.assertNotIn(b'[ERR]', self.target.before(self.console))

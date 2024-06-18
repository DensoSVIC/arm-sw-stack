#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.runtime.case import OERuntimeTestCase


class OpteeTest(OERuntimeTestCase):
    console = 'tf-a'

    def test_optee_normal(self):
        self.target.transition('on')
        self.target.expect(self.console,
                           r'Loading SP: SE Proxy',
                           timeout=30)

        self.target.expect(self.console,
                           r'Loading SP: SMM Gateway',
                           timeout=30)

        self.target.expect(self.console,
                           r'I/TC: Primary CPU switching to normal world boot',
                           timeout=60)

        self.assertNotIn(b'E/TC', self.target.before(self.console))

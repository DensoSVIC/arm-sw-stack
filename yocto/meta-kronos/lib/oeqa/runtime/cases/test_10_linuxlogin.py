#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.runtime.case import OERuntimeTestCase
from oeqa.core.decorator.depends import OETestDepends


class LinuxLoginTest(OERuntimeTestCase):
    @OETestDepends(['test_10_linuxboot.LinuxBootTest.test_linux_boot'])
    def test_linux_login(self):
        console = self.target.DEFAULT_CONSOLE

        # Login
        self.target.sendline(console, 'root')
        self.target.expect(console, r'root@.*:~#', timeout=300)

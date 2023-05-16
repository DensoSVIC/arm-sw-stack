#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.runtime.case import OERuntimeTestCase
from oeqa.core.decorator.depends import OETestDepends


class ParsecTest(OERuntimeTestCase):
    def run_cmd(self, cmd, timeout):
        return self.target.run(cmd, timeout=timeout)

    @OETestDepends(['test_10_linuxboot.LinuxBootTest.test_linux_boot'])
    def test_parsec(self):
        status, output = self.run_cmd('parsec-cli-tests.sh', timeout=1200)
        self.assertEqual(status, 0, msg='Parsec CLI tests failed.\n %s' % output)
        status, output = self.run_cmd('sync', timeout=120)
        self.assertEqual(status, 0, msg='Filesystem sync failed.\n %s' % output)

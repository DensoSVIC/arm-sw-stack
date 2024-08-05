#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.core.decorator.data import skipIfDataVar
from oeqa.core.decorator.depends import OETestDepends
from oeqa.runtime.case import OERuntimeTestCase


class PtestRunnerTest(OERuntimeTestCase):
    @OETestDepends(['test_10_linuxlogin.LinuxLoginTest.test_linux_login'])
    def test_ptestrunner(self):
        status, _ = self.target.run('ptest-runner '
                                    'container-engine-integration-tests '
                                    'parsec-simple-e2e-tests', timeout=3000)
        self.assertEqual(status, 0)

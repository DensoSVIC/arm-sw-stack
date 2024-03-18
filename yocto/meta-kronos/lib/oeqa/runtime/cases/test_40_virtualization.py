#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

import re
from oeqa.core.decorator.data import skipIfNotInDataVar, skipIfDataVar
from oeqa.core.decorator.depends import OETestDepends
from oeqa.runtime.case import OERuntimeTestCase
from oeqa.runtime.decorator.package import OEHasPackage
from oeqa.runtime.cases.fvp_devices import FvpDevicesTest
from oeqa.runtime.cases.test_40_gicv4_1 import GICv4Test
from oeqa.runtime.cases.test_40_parsec import ParsecTest
from oeqa.utils.xen_utils import XenUtils
from oeqa.utils.linux_terminal_utils import LinuxTermUtils
from oeqa.utils.kronos_config import KronosConfig


class DomUTest(OERuntimeTestCase):
    domu_hostname = None

    @classmethod
    def setUpClass(cls):
        super().setUpClass()

        cls.linux_prompt = rf'root@{cls.domu_hostname}:~#'
        linux_console = XenUtils.spawn_console_domu(cls.linux_prompt,
                                                    KronosConfig.dom0_prompt,
                                                    cls.domu_hostname,
                                                    cls.tc.target,
                                                    cls.tc.logger)
        cls.lt_utils = LinuxTermUtils(cls.tc, linux_console,
                                      cls.linux_prompt)

    def run_cmd(self, cmd, timeout=400, check=True):
        status, output = self.lt_utils.run(cmd, timeout)
        if status and check:
            self.fail("Command '%s' returned non-zero exit "
                      "status %d:\n%s" % (cmd, status, output))

        return status, output

    @classmethod
    def tearDownClass(cls):
        XenUtils.close_console_domu(cls.lt_utils, cls.linux_prompt,
                                    KronosConfig.dom0_prompt,
                                    cls.domu_hostname, cls.tc.logger)
        super().tearDownClass()


class DomU1Test(DomUTest):
    domu_hostname = r'domu1'


class DomU2Test(DomUTest):
    domu_hostname = r'domu2'

    @classmethod
    def setUpClass(cls):
        if int(cls.td.get('DOMU_INSTANCES', 0)) < 2:
            import unittest
            raise unittest.SkipTest("FVPDevicesTestDomU2 skipped because "
                                    "DomU2 is not generated in this build")
        super().setUpClass()


class DomUFVPDevicesTestOverrides:
    @OETestDepends(['test_10_linuxlogin.LinuxLoginTest.test_linux_login'])
    def test_rtc(self):
        self.skipTest("'rtc' not tested in DomU")

    @OETestDepends(['test_10_linuxlogin.LinuxLoginTest.test_linux_login'])
    def test_watchdog(self):
        self.skipTest("'watchdog' not tested in DomU")

    @OETestDepends(['test_10_linuxlogin.LinuxLoginTest.test_linux_login'])
    def test_virtiorng(self):
        self.skipTest("'virtiorng' not tested in DomU")


class FvpDevicesTestDomU1(DomU1Test,
                          DomUFVPDevicesTestOverrides,
                          FvpDevicesTest):
    pass


class FvpDevicesTestDomU2(DomU2Test,
                          DomUFVPDevicesTestOverrides,
                          FvpDevicesTest):
    pass


class ParsecDomU1Test(DomU1Test, ParsecTest):
    pass


class ParsecDomU2Test(DomU2Test, ParsecTest):
    pass


# Passthrough PCI AHCI SATA disk to DomU1
class GICv4DomU1Test(DomU1Test, GICv4Test):
    pass


class PtestRunnerDom0Test(OERuntimeTestCase):
    @OETestDepends(['test_10_linuxlogin.LinuxLoginTest.test_linux_login'])
    @OEHasPackage(['ptest-runner'])
    @skipIfDataVar('FREQUENCY', 'adhoc', 'Skip ptest-runner in adhoc builds')
    def test_ptestrunner(self):
        # Run ptest-runner
        status, _ = self.target.run('ptest-runner', timeout=2000)
        self.assertEqual(status, 0)

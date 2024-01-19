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


class DomUTest(OERuntimeTestCase):
    domu_hostname = None

    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        cls.linux_console = cls.tc.target.DEFAULT_CONSOLE
        # Use negative lookahead to match Dom0 prompt, so match every prompt
        # that is not of this guest
        cls.dom0_prompt = \
            rf'root@(?!{cls.domu_hostname})fvp-rd-kronos:~#'
        cls.linux_prompt = rf'root@{cls.domu_hostname}:~#'
        cls.console = cls.tc.target._get_terminal(cls.linux_console)
        XenUtils.enter_guest_from_dom0(cls.console, cls.dom0_prompt,
                                       cls.linux_prompt, cls.domu_hostname)

    def run_cmd(self, cmd, timeout=200, check=True):
        # Get the output of the command
        cmd_echo = re.compile(re.escape(cmd))
        self.target.sendline(self.linux_console, cmd)
        check_line = ""
        # Here we try to delete the Xen output for maximum 5 consecutive lines
        for _ in range(5):
            line = self.target.readline(self.linux_console)
            line = line.decode("utf-8", errors="replace").strip()
            check_line += re.sub(r'\(XEN\).*$', "", line).replace('\r\n', '')
            if cmd_echo.search(check_line):
                break

        if not cmd_echo.search(check_line):
            self.fail(f"Unable to check echo for command:\n'{cmd}'"
                      f"\nCommand line content: '{check_line}'")

        self.target.expect(self.linux_console,
                           self.linux_prompt, timeout=timeout)
        output = self.target.before(self.linux_console)
        output = output.decode("utf-8", errors="replace").strip()

        # Get the exit code of the command
        self.target.sendline(self.linux_console, 'echo $?')
        self.target.expect(self.linux_console, r'[0-9]+\r\r\n', timeout=40)
        matches = self.target.match(self.linux_console)
        status = int(matches[0].decode("utf-8", errors="replace").strip())
        self.target.expect(self.linux_console, self.linux_prompt, timeout=200)

        if status and check:
            self.fail("Command '%s' returned non-zero exit "
                      "status %d:\n%s" % (cmd, status, output))

        return status, output

    @classmethod
    def tearDownClass(cls):
        XenUtils.exit_guest_to_dom0(cls.console, cls.dom0_prompt,
                                    cls.linux_prompt, cls.domu_hostname)
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

#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

import re
from oeqa.core.decorator.data import skipIfNotInDataVar
from oeqa.core.decorator.depends import OETestDepends
from oeqa.runtime.case import OERuntimeTestCase
from oeqa.runtime.decorator.package import OEHasPackage
from oeqa.runtime.cases.test_20_bsp import BspTest
from oeqa.utils.xen_utils import XenUtils


class BspTestDomU1(BspTest):
    domu_hostname = r'domu1'

    def run_cmd(self, cmd):
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

        self.target.expect(self.linux_console, self.linux_prompt, timeout=200)
        output = self.target.before(self.linux_console)
        output = output.decode("utf-8", errors="replace").strip()

        # Get the exit code of the command
        self.target.sendline(self.linux_console, 'echo $?')
        self.target.expect(self.linux_console, r'[0-9]+\r\r\n', timeout=40)
        matches = self.target.match(self.linux_console)
        status = matches[0].decode("utf-8", errors="replace").strip()
        self.target.expect(self.linux_console, self.linux_prompt, timeout=200)

        return int(status), output

    @classmethod
    def setUpClass(cls):
        super(BspTestDomU1, cls).setUpClass()
        cls.linux_console = cls.tc.target.DEFAULT_CONSOLE
        # Use negative lookahead to match Dom0 prompt, so match every prompt
        # that is not of this guest
        cls.dom0_prompt = \
            rf'root@(?!{cls.domu_hostname})fvp-rd-kronos:~#'
        cls.linux_prompt = rf'root@{cls.domu_hostname}:~#'
        cls.console = cls.tc.target._get_terminal(cls.linux_console)
        XenUtils.enter_guest_from_dom0(cls.console, cls.dom0_prompt,
                                       cls.linux_prompt, cls.domu_hostname)

    @classmethod
    def tearDownClass(cls):
        XenUtils.exit_guest_to_dom0(cls.console, cls.dom0_prompt,
                                    cls.linux_prompt, cls.domu_hostname)
        super(BspTestDomU1, cls).tearDownClass()

    @skipIfNotInDataVar('TEST_BSP_DEVICES', 'rtc',
                        'rtc device not included in BSP tests')
    @OETestDepends(['test_10_linuxlogin.LinuxLoginTest.test_linux_login'])
    def test_rtc(self):
        self.skipTest("'rtc' not tested in DomU")

    @skipIfNotInDataVar('TEST_BSP_DEVICES', 'watchdog',
                        'watchdog device not included in BSP tests')
    @OETestDepends(['test_10_linuxlogin.LinuxLoginTest.test_linux_login'])
    def test_watchdog(self):
        self.skipTest("'watchdog' not tested in DomU")

    @skipIfNotInDataVar('TEST_BSP_DEVICES', 'virtiorng',
                        'virtiorng device not included in BSP tests')
    @OETestDepends(['test_10_linuxlogin.LinuxLoginTest.test_linux_login'])
    def test_virtiorng(self):
        self.skipTest("'virtiorng' not tested in DomU")


class BspTestDomU2(BspTestDomU1):
    domu_hostname = r'domu2'

    @classmethod
    def setUpClass(cls):
        if int(cls.td.get('DOMU_INSTANCES', 0)) < 2:
            import unittest
            raise unittest.SkipTest("BspTestDomU2 skipped because DomU2 is"
                                    " not generated in this build")
        super(BspTestDomU2, cls).setUpClass()

    @classmethod
    def tearDownClass(cls):
        super(BspTestDomU2, cls).tearDownClass()

    @skipIfNotInDataVar('TEST_BSP_DEVICES', 'networking',
                        'networking device not included in BSP tests')
    @OETestDepends(['test_10_linuxlogin.LinuxLoginTest.test_linux_login'])
    def test_networking(self):
        super().test_networking()

    @skipIfNotInDataVar('TEST_BSP_DEVICES', 'cpu_hotplug',
                        'smp device not included in BSP tests')
    @OETestDepends(['test_10_linuxlogin.LinuxLoginTest.test_linux_login'])
    def test_cpu_hotplug(self):
        super().test_cpu_hotplug()


class PtestRunnerDom0Test(OERuntimeTestCase):
    @OETestDepends(['test_10_linuxlogin.LinuxLoginTest.test_linux_login'])
    @OEHasPackage(['ptest-runner'])
    def test_ptestrunner(self):
        # Run ptest-runner
        status, _ = self.target.run('ptest-runner')
        self.assertEqual(status, 0)

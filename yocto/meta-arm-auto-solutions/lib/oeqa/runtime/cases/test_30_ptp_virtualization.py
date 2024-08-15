#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

import unittest

from oeqa.core.decorator.depends import OETestDepends
from oeqa.runtime.cases.test_30_ptp import PTPTest
from oeqa.utils.arm_auto_solutions_config import ArmAutoSolutionsConfig
from oeqa.utils.xen_utils import XenUtils


class PTPTestDom0(PTPTest):
    @OETestDepends([
        'test_30_hipc_virtualization.HIPCTestDomU1.test_ping_cluster2'])
    def test_ptp_linux_services(self):
        super().test_ptp_linux_services()

    @OETestDepends([
        'test_30_ptp_virtualization.PTPTestDom0.test_ptp_linux_services'])
    def test_ptp_si_clients(self):
        super().test_ptp_si_clients()


class PTPTestDomU1(PTPTest):
    domu_hostname = ArmAutoSolutionsConfig.domu1_hostname

    @classmethod
    def setUpClass(cls):
        if ('virtualization' not in cls.td.get('IMAGE_FEATURES', '').split()):
            raise unittest.SkipTest(f"{cls.__name__} skipped because"
                                    " 'virtualization' is not in"
                                    " IMAGE_FEATURES")
        super().setUpClass()
        cls.linuxptp_ifaces = ['ethsi0']
        cls.dom0_prompt = rf'root@(?!{cls.domu_hostname}){cls.hostname}:~#'
        cls.linux_prompt = rf'root@{cls.domu_hostname}:~#'
        XenUtils.enter_guest_from_dom0(cls.console, cls.dom0_prompt,
                                       cls.linux_prompt, cls.domu_hostname)

    @classmethod
    def tearDownClass(cls):
        # Cancel potentially pending 'journalctl -f' command
        cls.console.sendcontrol('C')
        cls.console.sendline()
        cls.console.expect(cls.linux_prompt, timeout=90)
        XenUtils.exit_guest_to_dom0(cls.console, cls.dom0_prompt,
                                    cls.linux_prompt, cls.domu_hostname)
        # Ensure network interface is not left in a down state
        cls.console.sendline(f'ifconfig {cls.domu_hostname}.ethsi0 up')
        cls.console.expect(cls.dom0_prompt, timeout=90)
        super().tearDownClass()

    @OETestDepends([
        'test_30_hipc_virtualization.HIPCTestDomU1.test_ping_cluster2'])
    def test_ptp_linux_services(self):
        super().test_ptp_linux_services()

    def test_ptp_si_clients(self):
        self.skipTest("PTP SI Clients not tested for DomU1")

    def linux_ctrl_c(self):
        self.target.sendcontrol(self.linux_console, 'C')
        self.target.sendline(self.linux_console)
        self.target.expect(self.linux_console, self.linux_prompt,
                           timeout=90)

    def check_linux_remote_clock(self):
        self.target.expect(self.linux_console,
                           # /* cspell:disable-next-line */
                           'selected best master clock '
                           r'[0-9a-f]+\.[0-9a-f]+\.[0-9a-f]+', timeout=90)
        self.target.expect(self.linux_console,
                           r'rms\s+\d+ max \d+ freq\s+(\+|-)\d+ '
                           r'\+\/-\s+\d+ delay\s+\d+ \+\/-\s+\d+',
                           timeout=90)

    @OETestDepends([
        'test_30_ptp_virtualization.PTPTestDomU1.test_ptp_linux_services'])
    def test_ptp_domu_client(self):
        # Perform an interface down / up and verify that PTP is sync-ed
        self.target.sendline(self.linux_console,
                             'journalctl | grep ptp4l | head -n 40')
        self.check_linux_remote_clock()
        self.target.expect(self.linux_console, self.linux_prompt, timeout=90)

        # Use SSH target to run command on dom0 while the console is in domu
        status, output = self.target.run(
            f'ifconfig {self.domu_hostname}.ethsi0 down')
        self.assertEqual(status, 0,
                         msg='Failed to bring down '
                             f'{self.domu_hostname}.ethsi0.\n{output}')

        self.target.sendline(self.linux_console, 'journalctl -f | grep ptp4l')
        self.target.expect(self.linux_console,
                           'selected local clock '
                           # /* cspell:disable-next-line */
                           r'[0-9a-f]+\.[0-9a-f]+\.[0-9a-f]+ as best master',
                           timeout=90)
        self.linux_ctrl_c()

        status, output = self.target.run(
            f'ifconfig {self.domu_hostname}.ethsi0 up')
        self.assertEqual(status, 0,
                         msg='Failed to bring up '
                             f'{self.domu_hostname}.ethsi0.\n{output}')

        self.target.sendline(self.linux_console, 'journalctl -f | grep ptp4l')
        self.check_linux_remote_clock()
        self.linux_ctrl_c()


class PTPTestDomU2(PTPTestDomU1):
    domu_hostname = ArmAutoSolutionsConfig.domu2_hostname

    @classmethod
    def setUpClass(cls):
        if int(cls.td.get('DOMU_INSTANCES', 0)) < 2:
            raise unittest.SkipTest("PTPTestDomU2 skipped because DomU2 is"
                                    " not generated in this build")
        super().setUpClass()

    @OETestDepends([
        'test_30_hipc_virtualization.HIPCTestDomU2.test_ping_cluster1'])
    def test_ptp_linux_services(self):
        super().test_ptp_linux_services()

    def test_ptp_si_clients(self):
        self.skipTest("PTP SI Clients not tested for DomU2")

    @OETestDepends([
        'test_30_ptp_virtualization.PTPTestDomU1.test_ptp_linux_services'])
    def test_ptp_domu_client(self):
        super().test_ptp_domu_client()

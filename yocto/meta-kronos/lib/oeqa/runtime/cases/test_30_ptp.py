#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.core.decorator.depends import OETestDepends
from oeqa.runtime.cases.test_30_ptp_base import PTPTestBase
from oeqa.utils.xen_utils import XenUtils


class PTPTest(PTPTestBase):
    nb_clusters = 3
    cl_console_template = 'safety_island_c'
    cl_iface_template = 'ethsi'

    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        cls.linuxptp_ifaces = cls.td.get('LINUXPTP_IFACES', '').split()

    @classmethod
    def tearDownClass(cls):
        # Ensure network interfaces are not left in a down state
        for i in range(cls.nb_clusters):
            cls.console.sendline(
                f'ifconfig {cls.cl_iface_template + str(i)} up')
            cls.console.expect(cls.linux_prompt, timeout=60)
        super().tearDownClass()

    @OETestDepends(['test_30_ptp.PTPTest.test_ptp_linux_services'])
    def test_ptp_si_clients(self):
        def cl_console(index):
            return self.cl_console_template + str(index)

        def cl_iface(index):
            return self.cl_iface_template + str(index)

        # Breakdown test into several loops in order to optimize wait time on
        # state machine changes.
        for i in range(self.nb_clusters):
            self.target.sendline(cl_console(i))
            self.target.expect(cl_console(i), self.si_prompt, timeout=60)

            self.check_zephyr_state(cl_console(i), True, 60)

            # Check for year 2XXX, as Zephyr gets initialized to 1970
            self.target.sendline(cl_console(i), 'date get')
            self.target.expect(cl_console(i),
                               r'2\d{3}-\d{2}-\d{2} \d{2}:\d{2}:\d{2} UTC',
                               timeout=60)
            self.target.expect(cl_console(i), self.si_prompt, timeout=60)

            self.target.sendline(self.linux_console,
                                 f'ifconfig {cl_iface(i)} down')
            self.target.expect(self.linux_console,
                               self.linux_prompt, timeout=60)

        for i in range(self.nb_clusters):
            self.target.expect(cl_console(i),
                               '<wrn> net_gptp: Reset Pdelay requests',
                               timeout=60)

            self.check_zephyr_state(cl_console(i), False)

            self.target.sendline(self.linux_console,
                                 f'ifconfig {cl_iface(i)} up')
            self.target.expect(self.linux_console,
                               self.linux_prompt, timeout=60)

        for i in range(self.nb_clusters):
            self.check_zephyr_state(cl_console(i), True, 60)


class PTPTestDomU1(PTPTestBase):
    domu_hostname = r'domu1'

    @classmethod
    def setUpClass(cls):
        if ('virtualization' not in cls.td.get('IMAGE_FEATURES', '').split()):
            import unittest
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
        cls.console.expect(cls.linux_prompt, timeout=60)
        XenUtils.exit_guest_to_dom0(cls.console, cls.dom0_prompt,
                                    cls.linux_prompt, cls.domu_hostname)
        # Ensure network interface is not left in a down state
        cls.console.sendline(f'ifconfig {cls.domu_hostname}.ethsi0 up')
        cls.console.expect(cls.dom0_prompt, timeout=60)
        super().tearDownClass()

    @OETestDepends(['test_30_ptp.PTPTestDomU1.test_ptp_linux_services'])
    def test_ptp_domu_client(self):
        self.target.sendline(self.linux_console,
                             'journalctl | grep ptp4l | head -n 40')
        self.check_linux_remote_clock()
        self.target.expect(self.linux_console, self.linux_prompt, timeout=60)

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
                           timeout=60)
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
    domu_hostname = r'domu2'

    @classmethod
    def setUpClass(cls):
        if int(cls.td.get('DOMU_INSTANCES', 0)) < 2:
            import unittest
            raise unittest.SkipTest("PTPTestDomU2 skipped because DomU2 is"
                                    " not generated in this build")
        super().setUpClass()

    @OETestDepends(['test_30_ptp.PTPTestDomU2.test_ptp_linux_services'])
    def test_ptp_domu_client(self):
        super().test_ptp_domu_client()

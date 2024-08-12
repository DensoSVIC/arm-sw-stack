#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.core.decorator.depends import OETestDepends
from oeqa.runtime.case import OERuntimeTestCase
from oeqa.utils.arm_auto_solutions_config import ArmAutoSolutionsConfig
from time import sleep


class PTPTest(OERuntimeTestCase):
    linux_console = 'default'
    hostname = ArmAutoSolutionsConfig.hostname
    linux_prompt = f'root@{hostname}:~#'
    si_prompt = r'uart:~\$ '
    linuxptp_ifaces = []
    nb_clusters = 3
    cl_console_template = 'safety_island_c'
    cl_iface_template = 'ethsi'

    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        cls.console = \
            cls.tc.target._get_terminal(cls.tc.target.DEFAULT_CONSOLE)
        cls.linuxptp_ifaces = cls.td.get('LINUXPTP_IFACES', '').split()

    @classmethod
    def tearDownClass(cls):
        # Ensure network interfaces are not left in a down state
        for i in range(cls.nb_clusters):
            cls.console.sendline(
                f'ifconfig {cls.cl_iface_template + str(i)} up')
            cls.console.expect(cls.linux_prompt, timeout=90)
        super().tearDownClass()

    def check_linux_service(self, iface):
        self.target.sendline(self.linux_console,
                             f'systemctl is-active ptp4l@{iface}.service')
        self.target.expect(self.linux_console,
                           r'(\r){1,2}\nactive(\r){1,2}\n' + self.linux_prompt,
                           timeout=90)

    def check_zephyr_state(self, cl_console, expect_sync, max_tries=1):
        def id_str(role):
            return rf'Port id    : 1 \({role}\)'

        def as_str(capable):
            return rf'AS capable : {capable}'

        # The port can be in different states after a de-sync, depending on the
        # timing. We only expect it not to be in "client" mode anymore.
        # /* cspell:disable-next-line */
        sync_role = 'SLAVE'
        desync_role = rf'[A-Z\-]+\b(?<!{sync_role})'
        id_pattern = [id_str(desync_role), id_str(sync_role)]
        as_pattern = [as_str('no'), as_str('yes')]

        tries = 0
        while tries < max_tries:
            self.target.sendline(cl_console, 'net gptp 1')
            id_match = self.target.expect(cl_console, id_pattern, timeout=90)
            as_match = self.target.expect(cl_console, as_pattern, timeout=90)
            self.target.expect(cl_console, self.si_prompt, timeout=90)

            if id_match == expect_sync and (as_match or not expect_sync):
                break

            tries += 1
            sleep(1)
        self.assertLess(tries, max_tries)

    @OETestDepends(['test_30_hipc.HIPCTestBase.test_hipc_cluster_cl1_cl2'])
    def test_ptp_linux_services(self):
        for iface in self.linuxptp_ifaces:
            self.check_linux_service(iface)

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
            self.target.expect(cl_console(i), self.si_prompt, timeout=90)

            self.check_zephyr_state(cl_console(i), True, 60)

            # Check for year 2XXX, as Zephyr gets initialized to 1970
            self.target.sendline(cl_console(i), 'date get')
            self.target.expect(cl_console(i),
                               r'2\d{3}-\d{2}-\d{2} \d{2}:\d{2}:\d{2} UTC',
                               timeout=90)
            self.target.expect(cl_console(i), self.si_prompt, timeout=90)

            self.target.sendline(self.linux_console,
                                 f'ifconfig {cl_iface(i)} down')
            self.target.expect(self.linux_console,
                               self.linux_prompt, timeout=90)

        for i in range(self.nb_clusters):
            self.target.expect(cl_console(i),
                               '<wrn> net_gptp: Reset Pdelay requests',
                               timeout=90)

            self.check_zephyr_state(cl_console(i), False)

            self.target.sendline(self.linux_console,
                                 f'ifconfig {cl_iface(i)} up')
            self.target.expect(self.linux_console,
                               self.linux_prompt, timeout=90)

        for i in range(self.nb_clusters):
            self.check_zephyr_state(cl_console(i), True, 60)

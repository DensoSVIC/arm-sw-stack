#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.core.decorator.depends import OETestDepends
from oeqa.runtime.case import OERuntimeTestCase
from time import sleep


class PTPTestBase(OERuntimeTestCase):
    linux_console = 'default'
    hostname = r'.*'
    linux_prompt = f'root@{hostname}:~#'
    si_prompt = r'uart:~\$ '
    linuxptp_ifaces = []

    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        cls.console = \
            cls.tc.target._get_terminal(cls.tc.target.DEFAULT_CONSOLE)

    def check_linux_service(self, iface):
        self.target.sendline(self.linux_console,
                             f'systemctl is-active ptp4l@{iface}.service')
        self.target.expect(self.linux_console,
                           r'(\r){1,2}\nactive(\r){1,2}\n' + self.linux_prompt,
                           timeout=60)

    def check_zephyr_state(self, cl_console, expect_sync, max_tries=1):
        def state(role):
            return rf' 1\s+0x[0-9a-f]+ \[1\]\s+{role}'

        # The port can be in different states after a de-sync, depending on the
        # timing. We only expect it not to be in "client" mode anymore.
        # /* cspell:disable-next-line */
        sync_role = 'SLAVE'
        desync_role = rf'[A-Z]+\b(?<!{sync_role})'
        pattern = [state(desync_role), state(sync_role)]

        tries = 0
        while tries < max_tries:
            self.target.sendline(cl_console, 'net gptp')
            match = self.target.expect(cl_console, pattern, timeout=60)
            self.target.expect(cl_console, self.si_prompt, timeout=60)

            if match == expect_sync:
                break

            tries += 1
            sleep(1)

        self.assertLess(tries, max_tries)

        if expect_sync:
            self.target.sendline(cl_console, 'net gptp 1')
            self.target.expect(cl_console, 'AS capable : yes', timeout=60)
            self.target.expect(cl_console, self.si_prompt, timeout=60)

    def linux_ctrl_c(self):
        self.target.sendcontrol(self.linux_console, 'C')
        self.target.sendline(self.linux_console)
        self.target.expect(self.linux_console, self.linux_prompt, timeout=60)

    def check_linux_remote_clock(self):
        self.target.expect(self.linux_console,
                           # /* cspell:disable-next-line */
                           'selected best master clock '
                           r'[0-9a-f]+\.[0-9a-f]+\.[0-9a-f]+', timeout=60)
        self.target.expect(self.linux_console,
                           r'rms\s+\d+ max \d+ freq\s+(\+|-)\d+ '
                           r'\+\/-\s+\d+ delay\s+\d+ \+\/-\s+\d+',
                           timeout=60)

    @OETestDepends(['test_10_linuxlogin.LinuxLoginTest.test_linux_login'])
    def test_ptp_linux_services(self):
        for iface in self.linuxptp_ifaces:
            self.check_linux_service(iface)

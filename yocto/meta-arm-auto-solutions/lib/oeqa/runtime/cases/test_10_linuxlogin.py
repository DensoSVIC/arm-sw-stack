#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.runtime.case import OERuntimeTestCase
from oeqa.core.decorator.depends import OETestDepends
from oeqa.utils.xen_utils import XenUtils


class LinuxLoginTest(OERuntimeTestCase):
    def setUp(self):
        super().setUp()
        self.timeout = int(self.td.get('TEST_FVP_LINUX_BOOT_TIMEOUT') or 900)
        self.console_name = self.target.DEFAULT_CONSOLE
        self.hostname = r'.*'

    def login_domus(self, domu_hostnames):
        console = self.target._get_terminal(self.console_name)
        for domu_h in domu_hostnames:
            dom0_prompt = rf'root@(?!{domu_h}){self.hostname}:~#'
            linux_prompt = rf'root@{domu_h}:~#'
            XenUtils.enter_guest_from_dom0(console, dom0_prompt,
                                           linux_prompt, domu_h)
            XenUtils.exit_guest_to_dom0(console, dom0_prompt,
                                        linux_prompt, domu_h)

    @OETestDepends(['test_10_linuxboot.LinuxBootTest.test_linux_boot'])
    def test_linux_login(self):
        self.target.transition("linux", self.timeout)

        # Login
        self.target.sendline(self.console_name, 'root')
        self.target.expect(self.console_name,
                           rf'root@{self.hostname}:~#',
                           timeout=300)

        # Ensure all services have started
        status, output = self.target.run('systemctl is-system-running --wait',
                                         timeout=2250)
        self.assertEqual(status, 0,
                         msg=f'Failed to get systemctl running.\n{output}')

        if 'virtualization' in self.td.get('IMAGE_FEATURES').split():
            # Wait for the domains to be fully booted
            domu_hostnames = ['domu1']

            if int(self.td.get('DOMU_INSTANCES', 0)) > 1:
                domu_hostnames.append('domu2')

            self.login_domus(domu_hostnames)

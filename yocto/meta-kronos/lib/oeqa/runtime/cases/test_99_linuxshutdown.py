#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.runtime.case import OERuntimeTestCase
from oeqa.core.decorator.depends import OETestDepends
from time import sleep


class LinuxShutdownTest(OERuntimeTestCase):
    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        cls.linux_console = cls.tc.target.DEFAULT_CONSOLE
        cls.rss_console = 'rss'

    @OETestDepends(['test_10_linuxlogin.LinuxLoginTest.test_linux_login'])
    def test_linux_shutdown(self):
        # Send a shutdown command from the linux console
        self.target.sendline(self.linux_console, 'shutdown now')
        self.target.expect(self.linux_console,
                           r'reboot: Power down',
                           timeout=600)
        self.target.expect(self.rss_console,
                           r'System shutdown complete',
                           timeout=300)
        # Give the FVP some time to shutdown
        sleep(30)
        # Leave the system in the correct state
        self.target.transition('off')

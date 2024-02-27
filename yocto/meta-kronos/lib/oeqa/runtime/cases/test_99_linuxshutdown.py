#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.runtime.case import OERuntimeTestCase
from oeqa.core.decorator.depends import OETestDepends
from time import sleep
import pexpect


class LinuxShutdownTest(OERuntimeTestCase):
    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        cls.linux_console = cls.tc.target.DEFAULT_CONSOLE
        cls.rss_console = 'rss'
        cls.scp_console = 'scp'
        cls.lcp_console = 'lcp'
        cls.tfa_console = 'tf-a'

    @OETestDepends(['test_10_linuxlogin.LinuxLoginTest.test_linux_login'])
    def test_linux_shutdown(self):
        # Send a shutdown command from the linux console
        self.target.sendline(self.linux_console, 'shutdown now')
        self.target.expect(self.linux_console,
                           r'reboot: Power down',
                           timeout=900)
        self.target.expect(self.rss_console,
                           r'System shutdown complete',
                           timeout=300)
        # Give the FVP some time to shutdown
        sleep(30)
        # Verify there were no errors in any of the consoles
        self.assertNotIn(b'[ERR]', self.target.before(self.rss_console))
        self.target.expect(self.scp_console, pexpect.EOF)
        self.assertNotIn(b'[ERROR]', self.target.before(self.scp_console))
        self.target.expect(self.lcp_console, pexpect.EOF)
        self.assertNotIn(b'[ERROR]', self.target.before(self.lcp_console))
        self.target.expect(self.tfa_console, pexpect.EOF)
        self.assertNotRegex(self.target.before(self.tfa_console),
                            br'ERROR:|E\/TC|PANIC')
        # Leave the system in the correct state
        self.target.transition('off')

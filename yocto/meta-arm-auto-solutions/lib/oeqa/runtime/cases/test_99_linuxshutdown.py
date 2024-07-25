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
        cls.rse_console = 'rse'
        cls.scp_console = 'scp'
        cls.tfa_console = 'tf-a'

    @OETestDepends(['test_10_linuxlogin.LinuxLoginTest.test_linux_login'])
    def test_linux_shutdown(self):
        # Send a shutdown command from the linux console
        self.target.sendline(self.linux_console, 'shutdown now')
        self.target.expect(self.linux_console,
                           r'System Power Off',
                           timeout=1350)
        self.target.expect(self.rse_console,
                           r'System shutdown complete',
                           timeout=450)

        # Verify there were no errors in any of the consoles
        self.assertNotIn(b'[ERR]', self.target.before(self.rse_console))

        # Timeout has been set to 300s to give FVP enough time to shutdown,
        # verify that there are no errors after 'System shutdown complete'
        self.target.expect(self.rse_console, pexpect.EOF, timeout=300)
        self.assertNotIn(b'[ERR]', self.target.before(self.rse_console))

        self.target.expect(self.scp_console, pexpect.EOF)
        self.assertNotIn(b'[ERROR]', self.target.before(self.scp_console))
        self.target.expect(self.tfa_console, pexpect.EOF)
        self.assertNotRegex(self.target.before(self.tfa_console),
                            br'ERROR:|E\/TC|PANIC')
        # Leave the system in the correct state
        self.target.transition('off')

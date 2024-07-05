#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.core.decorator.depends import OETestDepends
from oeqa.runtime.case import OERuntimeTestCase
import pexpect


class SystemReadyACSShutdownTest(OERuntimeTestCase):
    def setUp(self):
        self.console = self.td.get('ARM_SYSTEMREADY_ACS_CONSOLE')
        self.assertNotEqual(self.console, '',
                            msg='ARM_SYSTEMREADY_ACS_CONSOLE is not set')

    @OETestDepends(['arm_systemready_ir_acs.SystemReadyACSTest.test_acs'])
    def test_shutdown(self):
        self.target.sendline(self.console, r'shutdown now')
        self.target.expect(self.console, pexpect.EOF, timeout=300)
        self.logger.info('Shutdown complete')

        # Leave the system in the correct state
        self.target.transition('off')

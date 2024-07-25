#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.runtime.case import OERuntimeTestCase


class SecureFirmwareUpdateTest(OERuntimeTestCase):

    def setUp(self):
        super().setUp()
        self.uboot_console = self.target.DEFAULT_CONSOLE
        self.rse_console = 'rse'

    def test_securefirmwareupdate(self):
        # Turn on the FVP
        self.target.transition('on')
        self.target.expect(self.uboot_console,
                           r'Hit any key to stop autoboot:',
                           timeout=60)
        # Press Enter to stop U-Boot autoboot
        self.target.sendline(self.uboot_console, '')
        # Load the capsule file
        self.target.sendline(self.uboot_console,
                             'fatload mmc 0:1 0xa2000000 fw.cap')
        self.target.expect(self.uboot_console,
                           r'(\d+) bytes read in (\d+) ms',
                           timeout=60)
        # Start the capsule update
        self.target.sendline(self.uboot_console,
                             'efidebug capsule update -v 0xa2000000')
        self.target.expect(self.uboot_console,
                           r'EFI: FVP: Capsule shared buffer at 0x[0-9a-fA-F]+'
                           r' , size \d+ pages',
                           timeout=60)
        # Wait for update to be complete on RSE side
        self.target.expect(self.rse_console,
                           r'Flashing the image succeeded.',
                           timeout=2700)
        self.target.expect(self.rse_console,
                           r'Performing system reset...',
                           timeout=30)
        # Wait for the Primary Compute to reset
        self.target.expect(self.uboot_console,
                           r'Hit any key to stop autoboot:',
                           timeout=60)
        # Verify that TF-M booted from the correct boot index
        self.target.expect(self.rse_console,
                           r'get_fwu_agent_state: enter, boot_index = 1',
                           timeout=60)

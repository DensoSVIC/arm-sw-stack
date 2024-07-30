#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.runtime.case import OERuntimeTestCase
from oeqa.utils.arm_auto_solutions_config import ArmAutoSolutionsConfig


class SecureFirmwareUpdateTest(OERuntimeTestCase):

    def setUp(self):
        super().setUp()
        self.pc_console = self.target.DEFAULT_CONSOLE
        self.rse_console = 'rse'
        self.hostname = ArmAutoSolutionsConfig.hostname

    def test_securefirmwareupdate(self):
        # Turn on the FVP
        self.target.transition('on')
        self.target.expect(self.pc_console, r'fvp-rd-kronos login:',
                           timeout=1000)
        # Log in
        self.target.sendline(self.pc_console, 'root')
        self.target.expect(self.pc_console, rf'root@{self.hostname}:~#',
                           timeout=300)
        # Mount boot partition and MMC
        self.target.sendline(self.pc_console, r'mount /dev/vda1 /boot')
        self.target.expect(self.pc_console, rf'root@{self.hostname}:~#',
                           timeout=300)

        self.target.sendline(self.pc_console, r'mount /dev/mmcblk0p1 /mnt')
        self.target.expect(self.pc_console, rf'root@{self.hostname}:~#',
                           timeout=300)

        # Create and populate UpdateCapsule directory
        self.target.sendline(self.pc_console,
                             r'mkdir -p /boot/EFI/UpdateCapsule')
        self.target.expect(self.pc_console, rf'root@{self.hostname}:~#',
                           timeout=300)

        self.target.sendline(self.pc_console,
                             r'cp -f /mnt/fw.cap /boot/EFI/UpdateCapsule')

        self.target.expect(self.pc_console, rf'root@{self.hostname}:~#',
                           timeout=300)
        # Reboot
        self.target.sendline(self.pc_console, r'reboot')

        self.target.expect(self.pc_console,
                           r'EFI: FVP: Capsule shared buffer at 0x[0-9a-fA-F]+'
                           r' , size \d+ pages',
                           timeout=600)
        # Wait for update to be complete on RSE side
        self.target.expect(self.rse_console,
                           r'Flashing the image succeeded.',
                           timeout=3000)
        self.target.expect(self.rse_console,
                           r'Performing system reset...',
                           timeout=30)
        # Wait for the Primary Compute to reset
        self.target.expect(self.pc_console,
                           r'Hit any key to stop autoboot:',
                           timeout=60)
        # Verify that TF-M booted from the correct boot index
        self.target.expect(self.rse_console,
                           r'get_fwu_agent_state: enter, boot_index = 1',
                           timeout=60)

        # Expected errors on first reboot after FWU
        self.target.expect(self.rse_console,
                           b'flash_rss_capsule: version error',
                           timeout=1350)
        self.target.expect(self.rse_console,
                           b'flash_fip_capsule: version error',
                           timeout=1350)
        self.target.expect(self.rse_console,
                           b'Flashing the image Failed',
                           timeout=1350)

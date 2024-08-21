#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.runtime.case import OERuntimeTestCase


class UEFI_Secure_Boot_Test(OERuntimeTestCase):
    def setUp(self):
        super().setUp()
        self.console = self.target.DEFAULT_CONSOLE

    def test_unsigned_kernel_image(self):
        boot_partition = 'virtio 0:1'

        key_types = ["PK", "KEK", "db", "dbx"]
        unsigned_kernel_image = 'Image.unsigned'

        # Turn on the FVP.
        self.target.transition('on')

        # Wait to enter U-Boot command line.
        self.target.expect(self.console,
                           'Hit any key to stop autoboot:',
                           timeout=60)

        # Press Enter to stop U-Boot autoboot
        self.target.sendline(self.console, '')
        self.target.expect(self.console, '=>', timeout=10)

        # Check the existence of the authenticated variables.
        for key in key_types:
            self.target.sendline(self.console, f'env print -e -n {key}')
            self.target.expect(self.console,
                               f'EFI_.*?_GUID', timeout=40)

        self.target.expect(self.console, '=>', timeout=40)

        # Load the unsigned kernel image, then execute it.
        self.target.sendline(
            self.console,
            f'load {boot_partition} ${{loadaddr}} {unsigned_kernel_image}')
        self.target.expect(self.console, 'bytes read', timeout=10)
        self.target.expect(self.console, '=>', timeout=40)
        self.target.sendline(self.console, 'bootefi ${loadaddr}')
        self.target.expect(self.console, 'Image not authenticated', timeout=10)
        self.target.expect(self.console, '=>', timeout=10)

        # Leave the system in the correct state
        self.target.transition('off')

#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.runtime.case import OERuntimeTestCase


class LinuxBootTest(OERuntimeTestCase):
    def test_linux_boot(self):
        timeout = int(self.td.get('TEST_FVP_LINUX_BOOT_TIMEOUT') or 10*60)
        self.target.transition("linux", timeout)

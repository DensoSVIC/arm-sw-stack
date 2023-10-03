#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.runtime.case import OERuntimeTestCase
import pexpect


class FaultMgmtTest(OERuntimeTestCase):
    si_prompt = r'uart:~\$ '
    console = 'safety_island_c1'

    def test_tree(self):
        self.target.expect(self.console, self.si_prompt, timeout=60)

        self.target.sendline(self.console, "fault tree")
        self.target.expect(self.console, self.si_prompt, timeout=60)
        self.assertIn(b"fmu@2a510000", self.target.before(self.console))

    def test_system_fmu_internal_inject(self):
        for fault_id in ['0x1', '0x2', '0x8', '0x20']:
            self.target.expect(self.console, self.si_prompt, timeout=60)
            self.target.sendline(self.console,
                                 f"fault inject fmu@2a510000 {fault_id}")
            self.target.expect(self.console,
                               r"Fault received \(non-critical\): "
                               fr"{fault_id} on fmu@2a510000",
                               timeout=30)

    def test_system_fmu_internal_set_enabled(self):
        self.target.expect(self.console, self.si_prompt, timeout=60)
        self.target.sendline(self.console,
                             f"fault set_enabled fmu@2a510000 0x2 0")
        self.target.expect(self.console, 'Disabling fault', timeout=30)
        self.target.expect(self.console, self.si_prompt, timeout=30)
        self.target.sendline(self.console, f"fault inject fmu@2a510000 0x2")
        # Wait 10 seconds to ensure the fault is not triggered
        match = self.target.expect(self.console,
                                   ["Fault received", pexpect.TIMEOUT],
                                   timeout=10)
        self.assertEqual(match, 1)
        self.target.expect(self.console, self.si_prompt, timeout=30)

        # Re-enable the fault and ensure it is now received
        self.target.sendline(self.console,
                             f"fault set_enabled fmu@2a510000 0x2 1")
        self.target.expect(self.console, 'Enabling fault', timeout=30)
        self.target.expect(self.console,
                           "Fault received")

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

    def fmu_fault_clear(self):
        self.target.expect(self.console, self.si_prompt, timeout=60)
        self.target.sendline(self.console, "fault clear")
        self.target.expect(self.console, r"Done!", timeout=30)

    def test_tree(self):
        self.target.expect(self.console, self.si_prompt, timeout=60)

        self.target.sendline(self.console, "fault tree")
        self.target.expect(self.console, self.si_prompt, timeout=60)
        self.assertIn(b"fmu@2a510000", self.target.before(self.console))

    def test_system_fmu_internal_inject(self):
        self.fmu_fault_clear()
        for fault_id in ['0x1', '0x2', '0x8', '0x20']:
            self.target.expect(self.console, self.si_prompt, timeout=60)
            self.target.sendline(self.console,
                                 f"fault inject fmu@2a510000 {fault_id}")
            self.target.expect(self.console,
                               r"Fault received \(non-critical\): "
                               fr"{fault_id} on fmu@2a510000 : count 1",
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

    def test_fmu_fault_count(self):
        self.test_system_fmu_internal_inject()
        self.target.expect(self.console, self.si_prompt, timeout=60)
        self.target.sendline(self.console, "fault count")
        self.target.expect(self.console,
                           r"Number of fault reported: 4",
                           timeout=30)

    def test_fmu_fault_list(self):
        self.test_system_fmu_internal_inject()
        self.target.expect(self.console, self.si_prompt, timeout=60)
        self.target.sendline(self.console, "fault list")
        self.target.expect(self.console, r"Fault history:", timeout=30)
        for fault_id in ['0x1', '0x2', '0x8', '0x20']:
            self.target.expect(self.console,
                               r"Fault received \(non-critical\): "
                               fr"{fault_id} on fmu@2a510000 : count 1",
                               timeout=30)

    def test_fmu_fault_summary(self):
        self.test_system_fmu_internal_inject()
        self.target.expect(self.console, self.si_prompt, timeout=60)
        self.target.sendline(self.console, f"fault inject fmu@2a510000 0x20")
        self.target.expect(self.console, self.si_prompt, timeout=60)
        self.target.sendline(self.console, "fault summary")
        self.target.expect(self.console,
                           r"Number of fault reported: 5",
                           timeout=30)
        self.target.expect(self.console,
                           r"Most reported faults:\r\n"
                           r"Fault received \(non-critical\): "
                           fr"0x20 on fmu@2a510000 : count 2", timeout=30)
        self.target.expect(self.console,
                           r"Fault history:\s*\r?\n(?:Fault "
                           r"received \(non-critical\): (0x1|0x2|0x8) on "
                           r"fmu@2a510000 : count 1\s*\r?\n){3}Fault "
                           r"received \(non-critical\): 0x20 on "
                           r"fmu@2a510000 : count 2\r\n", timeout=30)

    def test_fmu_fault_clear(self):
        self.test_system_fmu_internal_inject()
        self.target.expect(self.console, self.si_prompt, timeout=60)
        self.target.sendline(self.console, "fault clear")
        self.target.expect(self.console, r"Erasing the storage...",
                           timeout=30)
        self.target.expect(self.console, r"Done!", timeout=30)
        self.target.expect(self.console, self.si_prompt, timeout=60)
        self.target.sendline(self.console, "fault list")
        self.target.expect(self.console, r"No fault reported",
                           timeout=30)

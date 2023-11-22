#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.core.decorator.depends import OETestDepends
from oeqa.runtime.case import OERuntimeTestCase
import pexpect

SYSTEM_FMU_INTERNAL_FAULTS = [
    '0x1',  # Clock error
    '0x2',  # Reset error
    '0x4',  # Lockstep error
    '0x8',  # Q channel error
    '0x10',  # APB parity error
    '0x20',  # DFT error
    '0x40',  # Incorrect APB key sequence
    '0x80',  # APB security error
    '0x100',  # APB access error
    '0x200',  # APB size error
]

GIC_FMU_FAULT_SAMPLE = [
    "0x100",  # GICD 0 - Clock error
    "0x4900",  # GICD 0 - External error 0
    "0x10000600",  # Wake 0 - QCH error
    "0x20000a00",  # SPI Collator ID 0 - External error 1
    "0x30000b00",  # CI 0 - DFT error
    "0x30001400",  # CI 0 - LPD error
    "0x40000800",  # ITS 0 - DGI AXIT CRC error
    "0x40001300",  # ITS 0 - COL SED in address bit
    "0x50000200",  # FMU 0 - FMU clock protection error
    "0x50000300",  # FMU 0 - FMU lockstep protection error
]


class FaultMgmtTest(OERuntimeTestCase):
    si_prompt = r'uart:~\$ '
    console = 'safety_island_c1'

    def fmu_fault_clear(self):
        self.target.expect(self.console, self.si_prompt, timeout=60)
        self.target.sendline(self.console, "fault clear")
        self.target.expect(self.console, r"Done!", timeout=120)

    def test_tree(self):
        self.target.expect(self.console, self.si_prompt, timeout=60)

        self.target.sendline(self.console, "fault tree")
        self.target.expect(self.console, self.si_prompt, timeout=60)
        tree = self.target.before(self.console)
        for fmu in [b"fmu@2a510000", b"fmu@2a570000"]:
            self.assertIn(fmu, tree)

    def test_system_fmu_internal_inject(self):
        self.fmu_fault_clear()
        for fault_id in SYSTEM_FMU_INTERNAL_FAULTS:
            self.target.expect(self.console, self.si_prompt, timeout=60)
            self.target.sendline(self.console,
                                 f"fault inject fmu@2a510000 {fault_id}")
            self.target.expect(self.console,
                               r"Fault received \(non-critical\): "
                               fr"{fault_id} on fmu@2a510000 : count 1",
                               timeout=90)

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

    @OETestDepends(['test_10_linuxboot.LinuxBootTest.test_linux_boot'])
    def test_gic_fmu_inject(self):
        for fault_id in GIC_FMU_FAULT_SAMPLE:
            # Enable fault
            self.target.expect(self.console, self.si_prompt, timeout=60)
            self.target.sendline(
                self.console,
                f"fault set_enabled fmu@2a570000 {fault_id} 1")
            self.target.expect(self.console, "Enabling fault", timeout=30)

            # Configure fault as non-critical and inject
            self.target.expect(self.console, self.si_prompt, timeout=30)
            self.target.sendline(
                self.console,
                f"fault set_critical fmu@2a570000 {fault_id} 0")
            self.target.expect(self.console, "Setting fault", timeout=30)
            self.target.expect(self.console, self.si_prompt, timeout=30)
            self.target.sendline(
                self.console,
                f"fault inject fmu@2a570000 {fault_id}")
            self.target.expect(self.console,
                               r"Fault received \(non-critical\): "
                               fr"{fault_id} on fmu@2a570000 : count 1",
                               timeout=90)

            # Configure fault as critical and inject
            self.target.expect(self.console, self.si_prompt, timeout=30)
            self.target.sendline(
                self.console,
                f"fault set_critical fmu@2a570000 {fault_id} 1")
            self.target.expect(self.console, "Setting fault", timeout=30)
            self.target.expect(self.console, self.si_prompt, timeout=30)
            self.target.sendline(self.console,
                                 f"fault inject fmu@2a570000 {fault_id}")
            self.target.expect(self.console,
                               r"Fault received \(critical\): "
                               fr"{fault_id} on fmu@2a570000 : count 1",
                               timeout=30)

    def test_fmu_fault_count(self):
        self.test_system_fmu_internal_inject()
        self.target.expect(self.console, self.si_prompt, timeout=60)
        self.target.sendline(self.console, "fault count")
        count = len(SYSTEM_FMU_INTERNAL_FAULTS)
        self.target.expect(self.console,
                           fr"Number of fault reported: {count}",
                           timeout=60)

    @OETestDepends(['test_10_linuxboot.LinuxBootTest.test_linux_boot'])
    def test_fmu_fault_list(self):
        self.test_system_fmu_internal_inject()
        self.test_gic_fmu_inject()
        self.target.expect(self.console, self.si_prompt, timeout=60)
        self.target.sendline(self.console, "fault list")
        self.target.expect(self.console, r"Fault history:", timeout=30)

        # Fault patterns for the address "2a510000" (only non-critical)
        for fault_id in SYSTEM_FMU_INTERNAL_FAULTS:
            pattern = (fr"Fault received \(non-critical\): {fault_id} on "
                       fr"fmu@2a510000 : count 1")
            self.target.expect(self.console, pattern, timeout=60)

        # For the address "2a570000" (critical and non-critical)
        for fault_id in GIC_FMU_FAULT_SAMPLE:
            non_critical_pattern = (fr"Fault received \(non-critical\): "
                                    fr"{fault_id} on fmu@2a570000 : count 1")
            critical_pattern = (fr"Fault received \(critical\): {fault_id} "
                                fr"on fmu@2a570000 : count 1")
            self.target.expect(self.console, non_critical_pattern, timeout=60)
            self.target.expect(self.console, critical_pattern, timeout=60)
        self.target.expect(self.console, self.si_prompt, timeout=60)
        self.target.sendline(self.console, f"fault inject fmu@2a510000 0x2")
        self.target.expect(self.console, self.si_prompt, timeout=90)
        self.target.sendline(self.console, "fault list 2")
        self.target.expect(self.console,
                           r"Fault received \(non-critical\): "
                           fr"0x2 on fmu@2a510000 : count 2",
                           timeout=90)

    def test_fmu_fault_summary(self):
        self.test_system_fmu_internal_inject()
        self.target.expect(self.console, self.si_prompt, timeout=60)
        self.target.sendline(self.console, f"fault inject fmu@2a510000 0x20")
        self.target.expect(self.console, self.si_prompt, timeout=60)
        self.target.sendline(self.console, "fault summary")
        count = len(SYSTEM_FMU_INTERNAL_FAULTS)
        self.target.expect(self.console,
                           fr"Number of fault reported: {count + 1}",
                           timeout=30)
        self.target.expect(self.console,
                           r"Most reported faults:\r\n",
                           timeout=60)
        self.target.expect(self.console,
                           r"Fault history:\s*\r?\n(?:Fault "
                           r"received \(non-critical\): [x\d]+ on "
                           fr"fmu@2a510000 : count \d+\s*\r?\n){{{count}}}",
                           timeout=60)

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
                           timeout=90)

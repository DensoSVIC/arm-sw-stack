#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.core.decorator.depends import OETestDepends
from oeqa.runtime.case import OERuntimeTestCase
from oeqa.utils.zephyr_shell import Shell
import os
import pexpect

FAULT_MGMT_CONSOLE = 'safety_island_c1'

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


class FaultMgmtTestBase(OERuntimeTestCase):

    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        cls.tc.target.transition('on')

    def setUp(self):
        super().setUp()
        self.console = FAULT_MGMT_CONSOLE
        self.shell = Shell(self.target, self.console, self.logger)
        self.shell.wait_for_prompt(timeout=60)

    def fmu_fault_clear(self):
        output = self.shell.exec_command("fault clear", timeout=120)
        self.assertIn("Erasing the storage...", output)
        self.assertIn("Done!", output)

    def fmu_fault_list(self, fault=""):
        return self.shell.exec_command(f"fault list {fault}", timeout=60)


class FaultMgmtTest(FaultMgmtTestBase):

    def test_tree(self):
        tree = self.shell.exec_command("fault tree")
        for fmu in ["fmu@2a510000", "fmu@2a570000", "ssu@2a500000"]:
            self.assertIn(fmu, tree)

    def test_system_fmu_internal_inject(self):
        self.fmu_fault_clear()
        for fault_id in SYSTEM_FMU_INTERNAL_FAULTS:
            self.shell.wait_for_prompt()
            self.target.sendline(self.console,
                                 f"fault inject fmu@2a510000 {fault_id}")
            self.target.expect(self.console,
                               r"Fault received \(non-critical\): "
                               fr"{fault_id} on fmu@2a510000",
                               timeout=90)
            self.target.expect(
                self.console,
                fr"Fault count for {fault_id} on fmu@2a510000: 1",
                timeout=300)

    def test_system_fmu_internal_set_enabled(self):
        output = self.shell.exec_command(
                "fault set_enabled fmu@2a510000 0x2 0")
        self.assertIn('Disabling fault', output)

        self.target.sendline(self.console, f"fault inject fmu@2a510000 0x2")
        # Wait 10 seconds to ensure the fault is not triggered
        match = self.target.expect(self.console,
                                   ["Fault received", pexpect.TIMEOUT],
                                   timeout=10)
        self.assertEqual(match, 1)
        self.shell.wait_for_prompt()

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
            output = self.shell.exec_command(
                f"fault set_enabled fmu@2a570000 {fault_id} 1")
            self.assertIn("Enabling fault", output)

            # Configure fault as non-critical and inject
            output = self.shell.exec_command(
                f"fault set_critical fmu@2a570000 {fault_id} 0", timeout=30)
            self.assertIn("Setting fault", output)
            self.target.sendline(self.console,
                                 f"fault inject fmu@2a570000 {fault_id}")
            self.target.expect(self.console,
                               r"Fault received \(non-critical\): "
                               fr"{fault_id} on fmu@2a570000",
                               timeout=90)
            self.target.expect(
                self.console,
                fr"Fault count for {fault_id} on fmu@2a570000: 1",
                timeout=300)

            # Configure fault as critical and inject
            self.shell.wait_for_prompt()
            output = self.shell.exec_command(
                f"fault set_critical fmu@2a570000 {fault_id} 1", timeout=30)
            self.assertIn("Setting fault", output)
            self.target.sendline(self.console,
                                 f"fault inject fmu@2a570000 {fault_id}")
            self.target.expect(self.console,
                               r"Fault received \(critical\): "
                               fr"{fault_id} on fmu@2a570000",
                               timeout=30)
            self.target.expect(
                self.console,
                fr"Fault count for {fault_id} on fmu@2a570000: 1",
                timeout=300)

    def test_fmu_fault_count(self):
        self.test_system_fmu_internal_inject()
        output = self.shell.exec_command("fault count", timeout=60)
        count = len(SYSTEM_FMU_INTERNAL_FAULTS)
        self.assertIn(f"Number of fault reported: {count}", output)

    @OETestDepends(['test_10_linuxboot.LinuxBootTest.test_linux_boot'])
    def test_fmu_fault_list(self):
        self.test_system_fmu_internal_inject()
        self.test_gic_fmu_inject()
        output = self.fmu_fault_list()

        # Fault patterns for the address "2a510000" (only non-critical)
        for fault_id in SYSTEM_FMU_INTERNAL_FAULTS:
            pattern = (f"Fault received (non-critical): {fault_id} on "
                       "fmu@2a510000 : count 1")
            self.assertIn(pattern, output)

        # For the address "2a570000" (critical and non-critical)
        for fault_id in GIC_FMU_FAULT_SAMPLE:
            non_critical_pattern = ("Fault received (non-critical): "
                                    f"{fault_id} on fmu@2a570000 : count 1")
            critical_pattern = (f"Fault received (critical): {fault_id} "
                                "on fmu@2a570000 : count 1")
            self.assertIn(non_critical_pattern, output)
            self.assertIn(critical_pattern, output)

        self.shell.exec_command("fault inject fmu@2a510000 0x2")
        output = self.fmu_fault_list("2")
        self.assertIn("Fault received (non-critical): "
                      "0x2 on fmu@2a510000 : count 2",
                      output)

    def filter_fault_history(self, output):
        lines = output.split('\n')
        cleaned_lines = []
        fault_history_section = False

        for line in lines:
            if line.startswith("Fault history:"):
                fault_history_section = True
            elif fault_history_section and \
                    line.startswith("Fault received (non-critical):"):
                cleaned_lines.append(line)

        return '\n'.join(cleaned_lines)

    def test_fmu_fault_summary(self):
        self.test_system_fmu_internal_inject()
        self.shell.exec_command("fault inject fmu@2a510000 0x20")
        output = self.shell.exec_command("fault summary", timeout=60)
        count = len(SYSTEM_FMU_INTERNAL_FAULTS)
        self.assertIn(f"Number of fault reported: {count + 1}", output)
        self.assertIn("Most reported faults:\r\n", output)
        filtered_output = self.filter_fault_history(output)
        self.assertRegex(filtered_output, r"^(?:Fault received "
                         r"\(non-critical\): 0x[0-9a-f]+ on "
                         fr"fmu@2a510000 : count \d+\s*\r?\n?){{{count}}}")

    def test_fmu_fault_clear(self):
        self.test_system_fmu_internal_inject()

        self.fmu_fault_clear()

        output = self.fmu_fault_list()
        self.assertIn("No fault reported", output)


class FaultMgmtSSUTest(FaultMgmtTestBase):

    def setUp(self):
        # Work around duplicate symlink creation so it can be recreated
        os.unlink(self.target.bootlog)
        self.logger.info('Resetting')
        self.target.transition('off')
        self.target.transition('on')
        # Call the parent setUp at this stage, after resetting
        super().setUp()

        # Ensure initial state is "TEST"
        output = self.shell.exec_command("fault safety_status ssu@2a500000")
        self.assertIn("TEST", output)

    def test_ssu_compl_ok(self):
        # TEST -> compl_ok -> SAFE
        output = self.shell.exec_command(
            "fault safety_control ssu@2a500000 compl_ok")
        self.assertIn("SAFE", output)

        # SAFE -> non-critical fault -> ERRN
        self.shell.exec_command("fault inject fmu@2a510000 2")
        output = self.shell.exec_command("fault safety_status ssu@2a500000")
        self.assertIn("ERRN", output)

        # ERRN -> compl_ok -> SAFE
        output = self.shell.exec_command(
            "fault safety_control ssu@2a500000 compl_ok")
        self.assertIn("SAFE", output)

        # SAFE -> critical fault -> ERRC
        self.shell.exec_command("fault set_enabled fmu@2a570000 0x200 1")
        self.shell.exec_command("fault set_critical fmu@2a570000 0x200 1")
        self.shell.exec_command("fault inject fmu@2a570000 0x200")
        output = self.shell.exec_command("fault safety_status ssu@2a500000")
        self.assertIn("ERRC", output)

        # ERRC is unrecoverable
        output = self.shell.exec_command(
            "fault safety_control ssu@2a500000 compl_ok")
        self.assertIn("ERRC", output)

    def test_ssu_nce_ok(self):
        # TEST -> nce_ok -> ERRN
        output = self.shell.exec_command(
            "fault safety_control ssu@2a500000 nce_ok")
        self.assertIn("ERRN", output)

        # ERRN -> nce_not_ok -> ERRC
        output = self.shell.exec_command(
            "fault safety_control ssu@2a500000 nce_not_ok")
        self.assertIn("ERRC", output)

        # ERRC is unrecoverable
        output = self.shell.exec_command(
            "fault safety_control ssu@2a500000 compl_ok")
        self.assertIn("ERRC", output)

    def test_ssu_ce_not_ok(self):
        # TEST -> ce_not_ok -> ERRC
        output = self.shell.exec_command(
            "fault safety_control ssu@2a500000 ce_not_ok")
        self.assertIn("ERRC", output)

        # ERRC is unrecoverable
        output = self.shell.exec_command(
            "fault safety_control ssu@2a500000 compl_ok")
        self.assertIn("ERRC", output)

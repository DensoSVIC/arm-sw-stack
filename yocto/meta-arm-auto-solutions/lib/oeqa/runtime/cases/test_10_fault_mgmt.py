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

ROOT_FMU = "fmu@2a510000"
SSU = "ssu@2a500000"


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

    def fmu_fault_set_enabled(self, fmu_dev, fault_id, enable):
        enable_flag = "1" if enable else "0"
        enable_text = "Enabling fault" if enable else "Disabling fault"
        self.target.sendline(self.console,
                             f"fault set_enabled {fmu_dev} {fault_id} "
                             f"{enable_flag}")
        self.target.expect(self.console, f"{enable_text}", timeout=90)

    def fmu_fault_set_critical(self, fmu_dev, fault_id, is_critical):
        critical_flag = "1" if is_critical else "0"
        critical_text = "critical" if is_critical else "non-critical"
        self.target.sendline(self.console,
                             f"fault set_critical {fmu_dev} {fault_id} "
                             f"{critical_flag}")
        self.target.expect(self.console,
                           f"Setting fault {fault_id} on device {fmu_dev} "
                           f"as {critical_text}", timeout=90)

    def fmu_fault_inject(self, fmu_dev, fault_id, is_critical):
        critical_text = r'critical' if is_critical else r'non-critical'
        self.target.sendline(self.console,
                             f"fault inject {fmu_dev} {fault_id}")
        self.target.expect(self.console,
                           rf"Fault received \({critical_text}\): "
                           fr"{fault_id} on {fmu_dev}",
                           timeout=90)
        self.target.expect(self.console,
                           fr"Fault count for {fault_id} on {fmu_dev}: (\d+)",
                           timeout=300)
        return int(self.target.match(self.console)[1])


class FaultMgmtTest(FaultMgmtTestBase):

    def test_tree(self):
        tree = self.shell.exec_command("fault tree")
        for fmu in [ROOT_FMU] + ["fmu@2a570000"] + [SSU]:
            self.assertIn(fmu, tree)

    def test_system_fmu_internal_inject(self):
        self.fmu_fault_clear()
        for fault_id in SYSTEM_FMU_INTERNAL_FAULTS:
            self.shell.wait_for_prompt()
            fault_count = self.fmu_fault_inject(ROOT_FMU, fault_id, False)
            self.assertGreater(fault_count, 0)

    def test_system_fmu_internal_set_enabled(self):
        self.fmu_fault_set_enabled(ROOT_FMU, "0x2", False)

        self.target.sendline(self.console, f"fault inject {ROOT_FMU} 0x2")
        # Wait 10 seconds to ensure the fault is not triggered
        match = self.target.expect(self.console,
                                   ["Fault received", pexpect.TIMEOUT],
                                   timeout=10)
        self.assertEqual(match, 1)
        self.shell.wait_for_prompt()

        # Re-enable the fault and ensure it is now received
        self.fmu_fault_set_enabled(ROOT_FMU, "0x2", True)
        self.target.expect(self.console, "Fault received")

    @OETestDepends(['test_10_linuxboot.LinuxBootTest.test_linux_boot'])
    def test_gic_fmu_inject(self):
        for fault_id in GIC_FMU_FAULT_SAMPLE:
            # Enable fault
            self.fmu_fault_set_enabled("fmu@2a570000", fault_id, True)

            # Configure fault as non-critical and inject
            self.fmu_fault_set_critical("fmu@2a570000", fault_id, False)
            fault_count = self.fmu_fault_inject("fmu@2a570000", fault_id,
                                                False)
            self.assertGreater(fault_count, 0)

            # Configure fault as critical and inject
            self.shell.wait_for_prompt()
            self.fmu_fault_set_critical("fmu@2a570000", fault_id, True)
            fault_count = self.fmu_fault_inject("fmu@2a570000", fault_id,
                                                True)
            self.assertGreater(fault_count, 0)

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

        # Fault patterns for the root fmu address (only non-critical)
        for fault_id in SYSTEM_FMU_INTERNAL_FAULTS:
            pattern = (f"Fault received (non-critical): {fault_id} on "
                       f"{ROOT_FMU} : count 1")
            self.assertIn(pattern, output)

        # For the address "2a570000" (critical and non-critical)
        for fault_id in GIC_FMU_FAULT_SAMPLE:
            non_critical_pattern = ("Fault received (non-critical): "
                                    f"{fault_id} on fmu@2a570000 : count 1")
            critical_pattern = (f"Fault received (critical): {fault_id} "
                                "on fmu@2a570000 : count 1")
            self.assertIn(non_critical_pattern, output)
            self.assertIn(critical_pattern, output)

        self.fmu_fault_inject(ROOT_FMU, "0x2", False)
        output = self.fmu_fault_list("2")
        self.assertIn("Fault received (non-critical): "
                      f"0x2 on {ROOT_FMU} : count 2",
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
        self.fmu_fault_inject(ROOT_FMU, "0x20", False)
        output = self.shell.exec_command("fault summary", timeout=60)
        count = len(SYSTEM_FMU_INTERNAL_FAULTS)
        self.assertIn(f"Number of fault reported: {count + 1}", output)
        self.assertIn("Most reported faults:\r\n", output)
        filtered_output = self.filter_fault_history(output)
        self.assertRegex(filtered_output, r"^(?:Fault received "
                         r"\(non-critical\): 0x[0-9a-f]+ on "
                         fr"{ROOT_FMU} : count \d+\s*\r?\n?){{{count}}}")

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
        output = self.shell.exec_command(f"fault safety_status {SSU}")
        self.assertIn("TEST", output)

    def test_ssu_compl_ok(self):
        # TEST -> compl_ok -> SAFE
        output = self.shell.exec_command(
            f"fault safety_control {SSU} compl_ok")
        self.assertIn("SAFE", output)

        # SAFE -> non-critical fault -> ERRN
        self.fmu_fault_inject(ROOT_FMU, "0x2", False)
        output = self.shell.exec_command(f"fault safety_status {SSU}")
        self.assertIn("ERRN", output)

        # ERRN -> compl_ok -> SAFE
        output = self.shell.exec_command(
            f"fault safety_control {SSU} compl_ok")
        self.assertIn("SAFE", output)

        # SAFE -> critical fault -> ERRC
        self.fmu_fault_set_enabled("fmu@2a570000", "0x200", True)
        self.fmu_fault_set_critical("fmu@2a570000", "0x200", True)
        self.fmu_fault_inject("fmu@2a570000", "0x200", True)
        output = self.shell.exec_command(f"fault safety_status {SSU}")
        self.assertIn("ERRC", output)

        # ERRC is unrecoverable
        output = self.shell.exec_command(
            f"fault safety_control {SSU} compl_ok")
        self.assertIn("ERRC", output)

    def test_ssu_nce_ok(self):
        # TEST -> nce_ok -> ERRN
        output = self.shell.exec_command(
            f"fault safety_control {SSU} nce_ok")
        self.assertIn("ERRN", output)

        # ERRN -> nce_not_ok -> ERRC
        output = self.shell.exec_command(
            f"fault safety_control {SSU} nce_not_ok")
        self.assertIn("ERRC", output)

        # ERRC is unrecoverable
        output = self.shell.exec_command(
            f"fault safety_control {SSU} compl_ok")
        self.assertIn("ERRC", output)

    def test_ssu_ce_not_ok(self):
        # TEST -> ce_not_ok -> ERRC
        output = self.shell.exec_command(
            f"fault safety_control {SSU} ce_not_ok")
        self.assertIn("ERRC", output)

        # ERRC is unrecoverable
        output = self.shell.exec_command(
            f"fault safety_control {SSU} compl_ok")
        self.assertIn("ERRC", output)

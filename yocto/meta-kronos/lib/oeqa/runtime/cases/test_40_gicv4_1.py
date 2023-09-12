#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.runtime.case import OERuntimeTestCase
from oeqa.core.decorator.depends import OETestDepends

class GICv4Test(OERuntimeTestCase):
    def run_cmd(self, cmd, timeout):
        return self.target.run(cmd, timeout=timeout)

    @OETestDepends(['test_10_linuxboot.LinuxBootTest.test_linux_boot'])
    def test_gicv4_1(self):
        status, output = self.run_cmd('lspci', timeout=300)
        self.assertEqual(status, 0,
                         msg='GICv4.1 tests failed on lspci.\n %s' % output)
        # Passthrough PCI AHCI SATA disk shall be re-assigned with
        # a new SBDF 00:00.0 in DomU1
        self.assertTrue(r'00:00.0' in output)
        self.logger.debug('lspci:')
        self.logger.debug(output)

        status, output = self.run_cmd('cat /proc/interrupts', timeout=300)
        self.assertEqual(status, 0,
                         msg='GICv4.1 tests failed on read /proc/interrupts.\n %s' % output)
        # Loop through each line
        for _, line in enumerate(output.splitlines()):
            # Search the one for ahci[0000:00:00.0]
            if r'ahci[0000:00:00.0]' in line:
                # Sum the 2rd element(CPU0) and 3rd element(CPU1) to
                # calculate the number of MSI-X interrupts from
                # ahci[0000:00:00.0] captured at domain boot-time
                self.assertGreater(int(line.split()[1]) + int(line.split()[2]), 0)
                self.logger.debug('vlpi_line:')
                self.logger.debug(line)
            # Search the one for IPI0(Rescheduling interrupts)
            elif r'IPI0:' in line:
                # Sum the 2rd element(CPU0) and 3rd element(CPU1) to
                # calculate the number of IPI0 interrupts captured at
                # domain boot-time
                self.assertGreater(int(line.split()[1]) + int(line.split()[2]), 0)
                self.logger.debug('ipi0_line:')
                self.logger.debug(line)

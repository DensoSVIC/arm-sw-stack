#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.runtime.case import OERuntimeTestCase
from oeqa.core.decorator.data import skipIfNotDataVar
import pexpect


class SIPSAArchTests(OERuntimeTestCase):

    def check_si_psa(self, console):
        self.target.expect(console,
                           r"TOTAL SIM ERROR : 0\r\n"
                           r"TOTAL FAILED    : 0\r\n", timeout=1800)

    @skipIfNotDataVar('ZEPHYR_APP_SAFETY_ISLAND_CL0', 'psa-storage-tests',
                      'Skip as ZEPHYR_APP_SAFETY_ISLAND_CL0 is not psa-storage-tests')
    def test_psa_si_cluster0(self):
        self.check_si_psa('safety_island_c0')

    @skipIfNotDataVar('ZEPHYR_APP_SAFETY_ISLAND_CL1', 'psa-storage-tests',
                      'Skip as ZEPHYR_APP_SAFETY_ISLAND_CL1 is not psa-storage-tests')
    def test_psa_si_cluster1(self):
        self.check_si_psa('safety_island_c1')

    @skipIfNotDataVar('ZEPHYR_APP_SAFETY_ISLAND_CL2', 'psa-storage-tests',
                      'Skip as ZEPHYR_APP_SAFETY_ISLAND_CL2 is not psa-storage-tests')
    def test_psa_si_cluster2(self):
        self.check_si_psa('safety_island_c2')

#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.runtime.case import OERuntimeTestCase
from oeqa.core.decorator.data import skipIfNotInDataVar
import pexpect


class SIPSAArchTests(OERuntimeTestCase):

    def check_si_psa(self, console):
        self.target.transition('on')
        self.target.expect(console,
                           r"TOTAL SIM ERROR : 0\r\n"
                           r"TOTAL FAILED    : 0\r\n", timeout=1800)

    # Only psa-related test variants execute,
    # including psa-storage-tests and psa-crypto-tests
    @skipIfNotInDataVar('ZEPHYR_APP_SAFETY_ISLAND_CL0', 'psa',
                        'Skip as ZEPHYR_APP_SAFETY_ISLAND_CL0 '
                        'is not psa-storage-tests or psa-crypto-tests')
    def test_psa_si_cluster0(self):
        self.check_si_psa('safety_island_c0')

    @skipIfNotInDataVar('ZEPHYR_APP_SAFETY_ISLAND_CL1', 'psa',
                        'Skip as ZEPHYR_APP_SAFETY_ISLAND_CL1 '
                        'is not psa-storage-tests or psa-crypto-tests')
    def test_psa_si_cluster1(self):
        self.check_si_psa('safety_island_c1')

    @skipIfNotInDataVar('ZEPHYR_APP_SAFETY_ISLAND_CL2', 'psa',
                        'Skip as ZEPHYR_APP_SAFETY_ISLAND_CL2 '
                        'is not psa-storage-tests or psa-crypto-tests')
    def test_psa_si_cluster2(self):
        self.check_si_psa('safety_island_c2')

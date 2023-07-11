#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

import re

from oeqa.core.decorator.data import skipIfNotFeature
from oeqa.core.decorator.depends import OETestDepends
from oeqa.runtime.cases.test_30_si0_ethernet0 import Ethernet0TestBase


class BridgeTest(Ethernet0TestBase):
    @skipIfNotFeature('si0-bridge-ethernet0',
                      'Test requires si0-bridge-ethernet0 to be in'
                       ' IMAGE_FEATURES')
    def test_si0_bridge_ethernet0(self):
        si_console = 'safety_island_c0'
        self.target.expect(si_console,
                           r'Bridge initialization complete',
                           timeout=60)

        output = self.target.before(si_console)
        matches = re.findall(br'Error: ', output)
        self.assertTrue(len(matches) == 0)

    @skipIfNotFeature('si0-bridge-ethernet0',
                      'Test requires si0-bridge-ethernet0 to be in'
                      ' IMAGE_FEATURES')
    @OETestDepends(['test_30_si0_bridge_ethernet0.BridgeTest'
                    '.test_si0_bridge_ethernet0'])
    def test_si1_bridge_ethernet0(self):
        si_console = 'safety_island_c1'
        host_port = self.td.get('FVP_SI1_BRIDGED_HOST_NETPORT')
        bound_ip = '192.168.10.1'
        self.ethernet0(si_console, host_port, bound_ip)

    @skipIfNotFeature('si0-bridge-ethernet0',
                      'Test requires si0-bridge-ethernet0 to be in'
                      ' IMAGE_FEATURES')
    @OETestDepends(['test_30_si0_bridge_ethernet0.BridgeTest'
                    '.test_si0_bridge_ethernet0'])
    def test_si2_bridge_ethernet0(self):
        si_console = 'safety_island_c2'
        host_port = self.td.get('FVP_SI2_BRIDGED_HOST_NETPORT')
        bound_ip = '192.168.10.2'
        self.ethernet0(si_console, host_port, bound_ip)

#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

import re
import subprocess
import os

from oeqa.core.decorator.data import skipIfNotFeature
from oeqa.core.decorator.depends import OETestDepends
from oeqa.runtime.case import OERuntimeTestCase


class Ethernet0TestBase(OERuntimeTestCase):
    si_prompt = r'uart:~\$ '

    def ethernet0(self, si_console, host_port, bound_ip):

        test_duration = int(self.td.get('SI0_ETHERNET0_TEST_DURATION', 5))

        # Zephyr as TCP server
        self.target.expect(si_console, self.si_prompt, timeout=50)
        self.target.sendline(si_console, f'zperf tcp download 5001 {bound_ip}')
        self.target.expect(si_console, 'TCP server started on port 5001',
                           timeout=30)
        # Run iperf on the host
        iperf_path = os.path.join(self.td.get('COMPONENTS_DIR'),
                                  self.td.get('BUILD_ARCH'),
                                  'iperf-native', 'usr', 'bin', 'iperf')
        completed = subprocess.run([iperf_path, '-l', '1K', '-c',
                                   'localhost', '-t', str(test_duration),
                                    '-p', host_port, '-r'],
                                   stdout=subprocess.PIPE,
                                   stderr=subprocess.STDOUT,
                                   check=True,
                                   timeout=100*test_duration)
        host_output = completed.stdout
        host_output = host_output.decode("utf-8", errors="replace").strip()
        self.logger.debug('host_output:')
        self.logger.debug(host_output)
        self.assertTrue(r'Client connecting to ' in host_output)
        matches = re.findall(r'(?:ERROR|WARN(ING)?): (.*)' '\n', host_output)
        self.assertEqual(len(matches), 0)

        test_patterns = [r' rate:',
                         r'<(?:err|wrn)> (.*)' '\n',
                         self.si_prompt]
        self.target.expect(si_console,
                           r'New TCP session started.' '\r\n',
                           timeout=50)
        passed = False
        while True:
            match_id = self.target.expect(si_console,
                                          test_patterns,
                                          timeout=50)
            self.assertNotEqual(match_id, 1)
            if match_id == 0:
                _ = self.target.match(si_console)
                passed = True
            elif match_id == 2 and passed:
                break


class BridgeTest(Ethernet0TestBase):
    @skipIfNotFeature('si0-bridge-ethernet0',
                      'Test requires si0-bridge-ethernet0 to be in'
                      ' IMAGE_FEATURES')
    def test_si0_bridge_ethernet0(self):
        timeout = int(self.td.get('TEST_FVP_LINUX_BOOT_TIMEOUT') or 10*60)
        self.target.transition("linux", timeout)
        si_console = 'safety_island_c0'
        self.target.expect(si_console,
                           r'Bridge initialization complete',
                           timeout=180)

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

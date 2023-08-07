#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

import re
import subprocess
import os

from oeqa.runtime.case import OERuntimeTestCase
from oeqa.core.decorator.data import skipIfNotFeature


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


class Si0Ethernet0Test(Ethernet0TestBase):
    @skipIfNotFeature('si0-ethernet0',
                      'Test requires si0-ethernet0 to be in IMAGE_FEATURES')
    def test_si0_ethernet0(self):
        si_console = 'safety_island_c0'
        self.target.expect(si_console, r'<inf> net_config: IPv4 address:',
                           timeout=50)

        host_port = self.td.get('FVP_SI0_ETHERNET0_HOST_NETPORT')
        bound_ip = '192.168.10.0'
        self.ethernet0(si_console, host_port, bound_ip)

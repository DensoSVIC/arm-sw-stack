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


class Si0Ethernet0Test(OERuntimeTestCase):
    si_console = 'safety_island_c0'
    si_prompt = r'uart:~\$ '

    @skipIfNotFeature('si0-ethernet0',
                      'Test requires si0-ethernet0 to be in IMAGE_FEATURES')
    def test_si0_ethernet0(self):

        test_duration = int(self.td.get('SI0_ETHERNET0_TEST_DURATION', 5))

        # Zephyr as TCP server
        self.target.expect(self.si_console, r'<inf> net_config: IPv4 address:',
                           timeout=50)
        self.target.sendline(self.si_console, 'zperf tcp download 5001')
        self.target.expect(self.si_console, 'TCP server started on port 5001',
                           timeout=30)
        # Run iperf on the host
        iperf_path = os.path.join(self.td.get('COMPONENTS_DIR'),
                                  self.td.get('BUILD_ARCH'),
                                  'iperf-native', 'usr', 'bin', 'iperf')
        host_port = self.td.get('FVP_SI0_ETHERNET0_HOST_NETPORT')
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
        self.target.expect(self.si_console,
                           r'New TCP session started.' '\r\n',
                           timeout=50)
        passed = False
        while True:
            match_id = self.target.expect(self.si_console,
                                          test_patterns,
                                          timeout=50)
            self.assertNotEqual(match_id, 1)
            if match_id == 0:
                _ = self.target.match(self.si_console)
                passed = True
            elif match_id == 2:
                if passed:
                    break

#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

import os
import pexpect

from oeqa.runtime.case import OERuntimeTestCase
from oeqa.core.decorator.depends import OETestDepends
from oeqa.utils.xen_utils import XenUtils


class ActuationTest(OERuntimeTestCase):
    linux_console = 'default'
    hostname = r'.*'
    si_console = 'safety_island_c0'
    domu_hostname = r'domu1'

    @classmethod
    def setUpClass(cls):
        super(ActuationTest, cls).setUpClass()
        cls.linux_prompt = rf'root@{cls.hostname}:~#'
        cls.host_log = \
            cls.tc.target._create_logfile("packet_analyzer_actuation")
        if 'virtualization' in cls.td.get('IMAGE_FEATURES').split():
            # Use negative lookahead to match Dom0 prompt, so match every
            # prompt that is not of this guest
            cls.dom0_prompt = \
                rf'root@(?!{cls.domu_hostname}){cls.hostname}:~#'
            cls.linux_prompt = rf'root@{cls.domu_hostname}:~#'
            cls.console = cls.tc.target._get_terminal(cls.linux_console)
            XenUtils.enter_guest_from_dom0(cls.console, cls.dom0_prompt,
                                           cls.linux_prompt, cls.domu_hostname)

    @classmethod
    def tearDownClass(cls):
        if 'virtualization' in cls.td.get('IMAGE_FEATURES').split():
            XenUtils.exit_guest_to_dom0(cls.console, cls.dom0_prompt,
                                        cls.linux_prompt, cls.domu_hostname)
        super(ActuationTest, cls).tearDownClass()

    @OETestDepends(['test_10_linuxlogin.LinuxLoginTest.test_linux_login'])
    def test_ping(self):
        self.target.expect(self.si_console,
                           r'Actuation Service initialized.',
                           timeout=30)

        self.target.sendline(self.linux_console, 'ping 192.168.0.1 -c 10')
        for _ in range(0, 10):
            self.target.expect(self.linux_console,
                               r'bytes from 192\.168\.0\.1',
                               timeout=10)
        self.target.expect(self.linux_console, self.linux_prompt, timeout=10)

    def get_analyzer_path(self):
        # Get packet analyzer package path
        return os.path.join(self.td.get('COMPONENTS_DIR'),
                            self.td.get('BUILD_ARCH'),
                            'packet-analyzer-native', 'usr', 'bin',
                            'actuation_packet_analyzer')

    def test_analyzer_help(self):
        analyzer = 'packet_analyzer/start_analyzer.py'

        host_output = pexpect.run(f'python3 {analyzer} -h',
                                  cwd=self.get_analyzer_path(), timeout=10)
        host_output = host_output.decode("utf-8", errors="replace").strip()
        self.logger.debug('host_output:')
        self.logger.debug(host_output)
        self.assertTrue(r'Start Packet Analyzer module' in host_output)

    def player_to_analyzer(self, run_all=True):
        if run_all:
            command_f = './data'
            test_recordings = '/usr/share/actuation_player'
            proc_timeout = 180
        else:
            command_f = './data/test_data'
            test_recordings = '/usr/share/actuation_player/test_data'
            proc_timeout = 120

        # localhost:FVP_SI0_ETHERNET0_HOST_NETPORT maps to 172.20.51.1:49152
        port = self.td.get('FVP_ACTUATION_HOST_ANALYZER_PORT')
        host = "localhost"
        analyzer = 'packet_analyzer/start_analyzer.py'
        cmd = f'python3 {analyzer} -L debug -p {port} -a {host} -c {command_f}'
        proc = pexpect.spawn(cmd, cwd=self.get_analyzer_path(),
                             logfile=self.host_log)
        proc.expect('Starting analyze, use Ctrl-C to stop the process',
                    timeout=10)
        self.target.expect(self.si_console,
                           'Accepted tcp connection from the Packet Analyzer',
                           timeout=5)

        cmd = f'actuation_player -p {test_recordings}'
        self.target.sendline(self.linux_console, cmd)
        self.target.expect(self.linux_console, 'Starting replay.',
                           timeout=10)
        self.target.expect(self.si_console,
                           r'[0-9]+:\s+-?\d+\.\d{4} \(m\/s\^2\) \|'
                           r'\s+-?\d+\.\d{4} \(rad\)',
                           timeout=10)
        proc.expect('All expected control packets received',
                    timeout=proc_timeout)
        proc.expect('Received fin ack from Actuation Service',
                    timeout=proc_timeout)
        proc.terminate()
        self.target.expect(self.si_console,
                           'Thread get_analyzer_handle performing a blocking '
                           'accept', timeout=10)
        self.target.sendline(self.linux_console, 'echo $?')
        self.target.expect(self.linux_console, r'0', timeout=proc_timeout)
        before = proc.before.decode("utf-8", errors="replace").strip()
        after = proc.after.decode("utf-8", errors="replace").strip()
        read = proc.read()
        read = read.decode("utf-8", errors="replace").strip()
        full_debug = f"cmd: {cmd}, after: <{after}>," + \
                     f"before: <{before}>," f"read: <{read}>"
        self.logger.debug('host_output:')
        self.logger.debug(full_debug)

        # Verify that AP is still running. This step is in lieu with a AP crash
        # that was observed during TCP client close from the Packet Analyzer
        self.target.sendline(self.linux_console, 'sleep 5')
        self.target.expect(self.linux_console, self.linux_prompt,
                           timeout=15)

    @OETestDepends(['test_30_actuation.ActuationTest.test_ping',
                    'test_30_actuation.ActuationTest.test_analyzer_help'])
    def test_player_to_analyzer_test_recording(self):
        self.player_to_analyzer(False)

    @OETestDepends(['test_30_actuation.ActuationTest.test_ping',
                    'test_30_actuation.ActuationTest.test_analyzer_help'])
    def test_player_to_analyzer_full_recording(self):
        if int(self.td.get('TEST_PLAYER_FULL_RECORDING', 0)) != 1:
            self.skipTest("Test skipped as FULL_RECORDING not requested")
        self.player_to_analyzer(True)

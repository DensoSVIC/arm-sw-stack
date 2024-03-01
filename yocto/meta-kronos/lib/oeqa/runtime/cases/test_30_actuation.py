#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

import os
import pexpect
import signal

from oeqa.runtime.case import OERuntimeTestCase
from oeqa.core.decorator.depends import OETestDepends
from oeqa.utils.xen_utils import XenUtils


class ActuationTest(OERuntimeTestCase):
    linux_console = 'default'
    hostname = r'.*'
    si_console = 'safety_island_c2'
    domu_hostname = r'domu1'

    @classmethod
    def setUpClass(cls):
        super(ActuationTest, cls).setUpClass()
        cls.linux_prompt = rf'root@{cls.hostname}:~#'
        cls.host_log = \
            cls.tc.target._create_logfile("packet_analyzer_actuation")
        cls.console = \
            cls.tc.target._get_terminal(cls.tc.target.DEFAULT_CONSOLE)
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
        # Cancel potentially pending 'actuation_player' command
        cls.console.sendcontrol('C')
        cls.console.sendline()
        cls.console.expect(cls.linux_prompt, timeout=60)
        if 'virtualization' in cls.td.get('IMAGE_FEATURES').split():
            XenUtils.exit_guest_to_dom0(cls.console, cls.dom0_prompt,
                                        cls.linux_prompt, cls.domu_hostname)
        super(ActuationTest, cls).tearDownClass()

    @OETestDepends(['test_10_linuxlogin.LinuxLoginTest.test_linux_login'])
    def test_ping(self):
        self.target.expect(self.si_console,
                           r'Actuation Service initialized.',
                           timeout=30)

        self.target.sendline(self.linux_console, 'ping 192.168.2.1 -c 10')
        for _ in range(0, 10):
            self.target.expect(self.linux_console,
                               r'bytes from 192\.168\.2\.1',
                               timeout=150)
        self.target.expect(self.linux_console, self.linux_prompt, timeout=120)

    def connect_to_host(self):
        # localhost:FVP_ACTUATION_HOST_ANALYZER_PORT maps to 192.168.10.2:49152
        command_f = './data'
        port = self.td.get('FVP_ACTUATION_HOST_ANALYZER_PORT')
        host = "localhost"
        cmd = f'start_analyzer -L debug -p {port} -a {host} -c {command_f}'
        proc = pexpect.spawn(cmd, logfile=self.host_log)
        proc.expect('Starting analyze, use Ctrl-C to stop the process',
                    timeout=10)
        return proc

    def test_analyzer_help(self):
        host_output = pexpect.run(f'start_analyzer -h', timeout=10)
        host_output = host_output.decode("utf-8", errors="replace").strip()
        self.logger.debug('host_output:')
        self.logger.debug(host_output)
        self.assertTrue(r'Start Packet Analyzer module' in host_output)

    @OETestDepends(['test_30_actuation.ActuationTest.test_ping',
                    'test_30_actuation.ActuationTest.test_analyzer_help'])
    def test_player_to_analyzer(self):
        test_recordings = '/usr/share/actuation_player'
        proc_timeout = 500

        proc = self.connect_to_host()
        cmd = f'actuation_player -p {test_recordings}'
        before = ""
        after = ""
        # Catch any exceptions thrown from PC or SI so that packet analyzer
        # can be terminated gracefully and then re-throw the exception.
        try:
            si_expect = 'Accepted tcp connection from the Packet Analyzer'
            self.target.expect(self.si_console, si_expect, timeout=20)
            self.target.sendline(self.linux_console, cmd)
            self.target.expect(self.linux_console, 'Starting replay.',
                               timeout=10)
            self.console.expect(self.linux_prompt, timeout=proc_timeout)
            self.target.sendline(self.linux_console, 'echo $?')
            self.target.expect(self.linux_console, r'0', timeout=5)

            # Verify that the replay messages from player have reached
            # actuation service
            self.target.expect(self.si_console,
                               r'[0-9]+:\s+-?\d+\.\d{4} \(m\/s\^2\) \|'
                               r'\s+-?\d+\.\d{4} \(rad\)',
                               timeout=10)

            # Verify if packet analyzer received all control packets from SI
            proc.expect('All expected control packets received',
                        timeout=proc_timeout)
            proc.expect('Received fin ack from Actuation Service',
                        timeout=proc_timeout)
            proc.expect('AnalyzerResult.SUCCESS', timeout=proc_timeout)
            before = proc.before.decode("utf-8", errors="replace").strip()
            after = proc.after.decode("utf-8", errors="replace").strip()
        except Exception as e:
            raise e
        finally:
            proc.kill(signal.SIGINT)
            read = proc.read()
            read = read.decode("utf-8", errors="replace").strip()
            self.logger.debug('host_output:')
            full_debug = f"cmd: {cmd}, after: <{after}>," + \
                         f"before: <{before}>," f"read: <{read}>"
            self.logger.debug(f"cmd: {cmd}, read: <{read}>")

        # Ensure player goes back to waiting for connection from analyzer
        si_expect = 'Thread get_analyzer_handle performing a blocking accept'
        self.target.expect(self.si_console, si_expect, timeout=10)

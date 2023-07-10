#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

import re

from oeqa.runtime.case import OERuntimeTestCase
from oeqa.core.decorator.depends import OETestDepends


class HIPCTestBase(OERuntimeTestCase):
    linux_console = 'default'
    hostname = r'.*'
    si_prompt = r'uart:~\$ '

    @classmethod
    def setUpClass(cls):
        super(HIPCTestBase, cls).setUpClass()

    def setUp(self):
        super().setUp()
        self.linux_prompt = rf'root@{self.hostname}:~#'

    def tearDown(self):
        super().tearDown()

    def vlan_subtest(self, cl_console, peer_addr, vlan_id):
        # Test that without VLAN configuration, ping does not work
        self.target.sendline(cl_console,
                             f'net vlan del {vlan_id} 1')
        self.target.expect(cl_console,
                           rf'VLAN tag {vlan_id} removed from interface 1'
                           r' \(.*\)',
                           timeout=150)
        self.target.sendline(cl_console,
                             f'net ping {peer_addr} -c 1')
        self.target.expect(cl_console, 'Ping timeout', timeout=120)

        # Test that with a vlan identifier different from the specification, it
        # does not work
        self.target.sendline(cl_console,
                             f'net vlan add {vlan_id + 10} 1')
        self.target.expect(cl_console,
                           rf'VLAN tag {vlan_id + 10} set to interface 1'
                           r' \(.*\)',
                           timeout=150)
        self.target.sendline(cl_console,
                             f'net ping {peer_addr} -c 1')
        self.target.expect(cl_console, 'Ping timeout', timeout=120)
        self.target.sendline(cl_console,
                             f'net vlan del {vlan_id + 10} 1')
        self.target.expect(cl_console,
                           rf'VLAN tag {vlan_id + 10} removed from interface 1'
                           r' \(.*\)',
                           timeout=150)

        # Set the original VLAN identifier
        self.target.sendline(cl_console,
                             f'net vlan add {vlan_id} 1')
        self.target.expect(cl_console,
                           rf'VLAN tag {vlan_id} set to interface 1 \(.*\)',
                           timeout=150)

    def ping(self, cl_addr, cl_console, peer_addr, vlan_id):
        self.target.sendline(cl_console)
        self.target.expect(cl_console, self.si_prompt, timeout=120)

        # Run connectivity test for VLAN settings
        self.vlan_subtest(cl_console, peer_addr, vlan_id)

        self.target.sendline(cl_console,
                             f'net ping {peer_addr} -c 10')
        for _ in range(0, 10):
            self.target.expect(cl_console,
                               rf'\d+ bytes from {re.escape(peer_addr)} to '
                               rf'{re.escape(cl_addr)}: icmp_seq=\d+ '
                               r'ttl=\d+ time=.* ms',
                               timeout=150)
        self.target.sendline(cl_console)
        self.target.expect(cl_console, self.si_prompt, timeout=120)

        self.target.sendline(self.linux_console, f'ping {cl_addr} -c 10')
        for _ in range(0, 10):
            self.target.expect(self.linux_console,
                               rf'\d+ bytes from {re.escape(cl_addr)}: '
                               r'seq=\d+ ttl=\d+ time=.* ms', timeout=120)
        self.target.sendline(self.linux_console)
        self.target.expect(self.linux_console, self.linux_prompt, timeout=120)

    def hipc(self, cl_addr, cl_console, peer_addr):
        """
        In hipc test case, since the throughput of zperf on FVP depends
        on host performance, we only check the minimum number of
        transferred bytes(100K) to guarantee the zperf test is OK, but
        not checking the maximum throughput on the specific platform.
        """
        def check_error_messages():
            # This function checks the console output between two expect
            # function call.
            # A list of error messages that are permitted to occur in either
            # iperf or zperf
            allowed_messages = [
                b'net_tcp: context->tcp == NULL',
            ]
            linux_output = self.target.before(self.linux_console)
            matches = re.findall(br'(?:ERROR|WARN(ING)?): (.*)' b'\r\n',
                                 linux_output)
            self.assertTrue(
                all(match in allowed_messages for match in matches))

            zephyr_output = self.target.before(cl_console)
            matches = re.findall(br'<(?:err|wrn)> (.*)' b'\r\n',
                                 zephyr_output)
            self.assertTrue(
                all(match in allowed_messages for match in matches))

        def test_zephyr_udp_server(test_duration, connections_number=1):
            self.target.sendline(
                self.linux_console,
                f'iperf -u -c {cl_addr} -t {test_duration} -b 100K -l 1438'
                f' -P {connections_number}')
            session_end_timeout = 300 * test_duration * connections_number
            self.target.expect(self.linux_console, 'Client connecting to ',
                               timeout=session_end_timeout)

            for _ in range(0, connections_number):
                self.target.expect(cl_console, r'End of session!\r\n',
                                   timeout=session_end_timeout)
                self.target.expect(cl_console,
                                   r'received packets:\s*(\d+)\r\n',
                                   timeout=10)
                self.assertGreater(int(self.target.match(cl_console)[1]),
                                   10)
                self.target.expect(cl_console,
                                   r'nb packets lost:\s*0\r\n',
                                   timeout=10)
                self.target.expect(cl_console,
                                   r'nb packets outorder:\s*0\r\n',
                                   timeout=10)
                check_error_messages()

            self.target.sendline(self.linux_console)
            self.target.expect(self.linux_console, self.linux_prompt,
                               timeout=session_end_timeout)
            self.target.sendline(cl_console)
            self.target.expect(cl_console, self.si_prompt, timeout=120)

        def test_zephyr_tcp_server(test_duration, connections_number=1):
            self.target.sendline(self.linux_console,
                                 f'iperf -c {cl_addr} -t {test_duration}'
                                 f' -P {connections_number}')
            session_end_timeout = 300 * test_duration * connections_number
            self.target.expect(self.linux_console, 'Client connecting to ',
                               timeout=session_end_timeout)
            for _ in range(0, connections_number):
                self.target.expect(cl_console, r'TCP session ended\r\n',
                                   timeout=session_end_timeout)
                check_error_messages()

            self.target.sendline(self.linux_console)
            self.target.expect(self.linux_console, self.linux_prompt,
                               timeout=session_end_timeout)
            self.target.sendline(cl_console)
            self.target.expect(cl_console, self.si_prompt, timeout=120)

        test_duration = int(self.td.get('HIPC_PER_TEST_DURATION', 3))

        # The variable HIPC_TEST_PARALLEL_CONNS_SEQ contains the sequence of
        # how many multiple connections should be tested
        connections_var = str(self.td.get('HIPC_TEST_PARALLEL_CONNS_SEQ', "1"))

        try:
            connections = [
                int(n) for n in connections_var.replace(" ", "").split(",")]
        except ValueError:
            raise ValueError("Error parsing HIPC_TEST_PARALLEL_CONNS_SEQ")

        # Zephyr as UDP server handling multiple parallel connections
        self.target.sendline(cl_console, 'zperf udp download')
        self.target.expect(cl_console, 'UDP server started on port 5001',
                           timeout=120)
        try:
            for parallel_connections in connections:
                test_zephyr_udp_server(test_duration, parallel_connections)
        finally:
            self.target.sendline(cl_console, 'zperf udp download stop')
            self.target.expect(cl_console, 'UDP server stopped', timeout=120)

        # Zephyr as TCP server handling multiple parallel connections
        self.target.sendline(cl_console, 'zperf tcp download')
        self.target.expect(cl_console, 'TCP server started on port 5001',
                           timeout=120)
        try:
            for parallel_connections in connections:
                test_zephyr_tcp_server(test_duration, parallel_connections)
        finally:
            self.target.sendline(cl_console, 'zperf tcp download stop')
            self.target.expect(cl_console, 'TCP server stopped', timeout=120)

        # Zephyr as UDP client
        self.target.sendline(self.linux_console, 'iperf -u -s -P 1')
        self.target.expect(self.linux_console,
                           'Server listening on UDP port 5001', timeout=120)
        # zperf udp upload <dest ip> <dest port> <duration> <packet size>
        # <bandwidth>
        self.target.sendline(
            cl_console,
            f'zperf udp upload {peer_addr} 5001 {test_duration} 1k 100K')
        self.target.expect(cl_console, r'Num packets:\s*(\d+)\s',
                           timeout=(100 * test_duration))
        self.assertGreater(int(self.target.match(cl_console)[1]), 10)
        # During this test, it can happen that error messages are shown before
        # the test ends, but the test itself is succeeding, check that no error
        # is found before the end of the test.
        check_error_messages()
        self.target.expect(cl_console,
                           r'Num packets out order:\s*0\r\n',
                           timeout=120)
        self.target.expect(cl_console,
                           r'Num packets lost:\s*0\r\n',
                           timeout=120)
        self.target.sendline(cl_console)
        self.target.expect(cl_console, self.si_prompt, timeout=120)
        self.target.sendline(self.linux_console)
        self.target.expect(self.linux_console, self.linux_prompt, timeout=120)
        check_error_messages()

        # Zephyr as TCP client
        self.target.sendline(self.linux_console, 'iperf -s -P 1')
        self.target.expect(self.linux_console,
                           'Server listening on TCP port 5001', timeout=120)
        # zperf tcp upload <dest ip> <dest port> <duration> <packet size>
        self.target.sendline(
            cl_console,
            f'zperf tcp upload {peer_addr} 5001 {test_duration} 1k')
        self.target.expect(cl_console, r'Num packets:\s*(\d+)\r\n',
                           timeout=(300 * test_duration))
        # During this test, it can happen that error messages are shown before
        # the test ends, but the test itself is succeeding, check that no error
        # is found before the end of the test.
        check_error_messages()
        self.assertGreater(int(self.target.match(cl_console)[1]), 10)
        self.target.expect(cl_console,
                           r'Num errors:\s*0 \(retry or fail\)',
                           timeout=100)
        self.target.sendline(cl_console)
        self.target.expect(cl_console, self.si_prompt, timeout=150)
        self.target.sendline(self.linux_console)
        self.target.expect(self.linux_console, self.linux_prompt, timeout=120)
        check_error_messages()

    @OETestDepends(['test_10_linuxlogin.LinuxLoginTest.test_linux_login'])
    def test_ping_cluster0(self):
        self.ping(r'192.168.0.1', 'safety_island_c0', r'192.168.0.2', 100)

    @OETestDepends(['test_10_linuxlogin.LinuxLoginTest.test_linux_login'])
    def test_ping_cluster1(self):
        self.ping(r'192.168.1.1', 'safety_island_c1', r'192.168.1.2', 200)

    @OETestDepends(['test_10_linuxlogin.LinuxLoginTest.test_linux_login'])
    def test_ping_cluster2(self):
        self.ping(r'192.168.2.1', 'safety_island_c2', r'192.168.2.2', 300)

    @OETestDepends(['test_30_hipc.HIPCTestBase.test_ping_cluster0'])
    def test_hipc_cluster0(self):
        self.hipc(r'192.168.0.1', 'safety_island_c0', r'192.168.0.2')

    @OETestDepends(['test_30_hipc.HIPCTestBase.test_ping_cluster1'])
    def test_hipc_cluster1(self):
        self.hipc(r'192.168.1.1', 'safety_island_c1', r'192.168.1.2')

    @OETestDepends(['test_30_hipc.HIPCTestBase.test_ping_cluster2'])
    def test_hipc_cluster2(self):
        self.hipc(r'192.168.2.1', 'safety_island_c2', r'192.168.2.2')

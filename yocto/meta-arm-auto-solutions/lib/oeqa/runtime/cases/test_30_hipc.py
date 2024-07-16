#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

import re

from oeqa.runtime.case import OERuntimeTestCase
from oeqa.core.decorator.depends import OETestDepends
from oeqa.core.decorator.data import skipIfDataVar
from oeqa.utils.arm_auto_solutions_config import ArmAutoSolutionsConfig

TCP_TEST_DURATION = 1
UDP_TEST_DURATION = 3


class HIPCTestBase(OERuntimeTestCase):
    linux_console = 'default'
    hostname = ArmAutoSolutionsConfig.hostname
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
                           r' \(0x[0-9a-fA-F]+\)',
                           timeout=225)
        self.target.sendline(cl_console,
                             f'net ping {peer_addr} -c 1')
        self.target.expect(cl_console, 'Ping timeout', timeout=180)

        # Test that with a vlan identifier different from the specification, it
        # does not work
        self.target.sendline(cl_console,
                             f'net vlan add {vlan_id + 10} 1')
        self.target.expect(cl_console,
                           rf'VLAN tag {vlan_id + 10} set to interface 1'
                           r' \(0x[0-9a-fA-F]+\)',
                           timeout=225)
        self.target.sendline(cl_console,
                             f'net ping {peer_addr} -c 1')
        self.target.expect(cl_console, 'Ping timeout', timeout=180)
        self.target.sendline(cl_console,
                             f'net vlan del {vlan_id + 10} 1')
        self.target.expect(cl_console,
                           rf'VLAN tag {vlan_id + 10} removed from interface 1'
                           r' \(0x[0-9a-fA-F]+\)',
                           timeout=225)

        # Set the original VLAN identifier
        self.target.sendline(cl_console,
                             f'net vlan add {vlan_id} 1')
        self.target.expect(cl_console,
                           rf'VLAN tag {vlan_id} set to interface 1'
                           r' \(0x[0-9a-fA-F]+\)',
                           timeout=225)

    def ping(self, cl_addr, cl_console, peer_addr, vlan_id):

        # For this test, 'vlan_id' must be one of the
        # following valid values: 100, 200, or 300.
        # If 'vlan_id' is set to -1, it will exclude all VLAN sub test cases.
        # Additionally, if 'vlan_id' is set to -1,
        # it will also disable the A <> R ping and enable R <> R

        self.target.sendline(cl_console)
        self.target.expect(cl_console, self.si_prompt, timeout=180)

        # Run connectivity test for VLAN settings
        if vlan_id != -1:
            self.vlan_subtest(cl_console, peer_addr, vlan_id)

        self.target.sendline(cl_console,
                             f'net ping {peer_addr} -c 10')
        for _ in range(0, 10):
            self.target.expect(cl_console,
                               rf'\d+ bytes from {re.escape(peer_addr)} to '
                               rf'{re.escape(cl_addr)}: icmp_seq=\d+ '
                               r'ttl=\d+ time=\d+(\.\d+)? ms',
                               timeout=350)
        self.target.sendline(cl_console)
        self.target.expect(cl_console, self.si_prompt, timeout=180)

        if vlan_id != -1:
            self.target.sendline(self.linux_console, f'ping {cl_addr} -c 10')
            for _ in range(0, 10):
                self.target.expect(self.linux_console,
                                   rf'\d+ bytes from {re.escape(cl_addr)}: '
                                   r'seq=\d+ ttl=\d+ time=\d+(\.\d+)? ms',
                                   timeout=180)
            self.target.sendline(self.linux_console)
            self.target.expect(self.linux_console, self.linux_prompt,
                               timeout=180)

    def check_error_messages(self, server, client):
        def error_check(allow_list, messages):
            error_lines = []
            # This regex matches linux shell possible errors:
            # 'ERROR: [...]', 'WARN: [...]', 'WARNING: [...]'
            # and also zephyr shell possible errors:
            # '<err> [...]', '<wrn> [...]'
            msg_regex = (br'(ERROR:|WARN:|WARNING:|<err>|<wrn>)\s'
                         br'(?P<msg>.*)\r\n')
            matches = re.finditer(msg_regex, messages)

            for line in [match.group("msg") for match in matches]:
                if not any(re.match(allow, line) for allow in allow_list):
                    decode_line = line.decode("utf-8",
                                              errors="replace").strip()
                    error_lines.append(decode_line)

            return error_lines

        # This function checks the console output between two expect
        # function call.
        # A list of error messages that are permitted to occur in either
        # iperf or zperf, this list can contain regex, please write them to
        # match the whole error message and not only part of it.
        allowed_messages = [
            br'net_tcp: context->tcp == NULL',
            br'net_gptp: Not AS capable: \d+ ns > \d+ ns',
        ]
        server_output = self.target.before(server)
        errors = error_check(allowed_messages, server_output)
        self.assertEqual(len(errors), 0,
                         msg="Errors found in the server console:\n{}"
                         .format('\n'.join(errors)))

        client_output = self.target.before(client)
        errors = error_check(allowed_messages, client_output)
        self.assertEqual(len(errors), 0,
                         msg="Errors found in the client console:\n{}"
                         .format('\n'.join(errors)))

    def hipc(self, cl_addr, cl_console, peer_addr):
        """
        In hipc test case, since the throughput of zperf on FVP depends
        on host performance, we only check the minimum number of
        transferred bytes(100K) to guarantee the zperf test is OK, but
        not checking the maximum throughput on the specific platform.
        """

        def test_zephyr_udp_server(connections_number=1):
            self.target.sendline(
                self.linux_console,
                f'iperf -u -c {cl_addr} -t {UDP_TEST_DURATION} -b 100K -l 1438'
                f' -P {connections_number}')
            session_end_timeout = 300 * UDP_TEST_DURATION * connections_number
            self.target.expect(self.linux_console, 'Client connecting to ',
                               timeout=session_end_timeout)

            for _ in range(0, connections_number):
                self.target.expect(cl_console, r'End of session!\r\n',
                                   timeout=session_end_timeout)
                self.target.expect(cl_console,
                                   r'received packets:\s*\d+\r\n',
                                   timeout=15)
                self.target.expect(cl_console,
                                   r'nb packets lost:\s*0\r\n',
                                   timeout=15)
                self.target.expect(cl_console,
                                   r'nb packets outorder:\s*0\r\n',
                                   timeout=15)
                self.check_error_messages(self.linux_console, cl_console)

            self.target.sendline(self.linux_console)
            self.target.expect(self.linux_console, self.linux_prompt,
                               timeout=session_end_timeout)
            self.target.sendline(cl_console)
            self.target.expect(cl_console, self.si_prompt, timeout=180)

        def test_zephyr_tcp_server(connections_number=1):
            self.target.sendline(self.linux_console,
                                 f'iperf -c {cl_addr} -t {TCP_TEST_DURATION}'
                                 f' -P {connections_number}')
            session_end_timeout = 600 * TCP_TEST_DURATION * connections_number
            self.target.expect(self.linux_console, 'Client connecting to ',
                               timeout=session_end_timeout)
            for _ in range(0, connections_number):
                self.target.expect(cl_console, r'TCP session ended\r\n',
                                   timeout=session_end_timeout)
                self.check_error_messages(self.linux_console, cl_console)

            self.target.sendline(self.linux_console)
            self.target.expect(self.linux_console, self.linux_prompt,
                               timeout=session_end_timeout)
            self.target.sendline(cl_console)
            self.target.expect(cl_console, self.si_prompt, timeout=180)

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
                           timeout=180)
        try:
            for parallel_connections in connections:
                test_zephyr_udp_server(parallel_connections)
        finally:
            self.target.sendline(cl_console, 'zperf udp download stop')
            self.target.expect(cl_console, 'UDP server stopped', timeout=180)

        # Zephyr as TCP server handling multiple parallel connections
        self.target.sendline(cl_console, 'zperf tcp download')
        self.target.expect(cl_console, 'TCP server started on port 5001',
                           timeout=180)
        try:
            for parallel_connections in connections:
                test_zephyr_tcp_server(parallel_connections)
        finally:
            self.target.sendline(cl_console, 'zperf tcp download stop')
            self.target.expect(cl_console, 'TCP server stopped', timeout=180)

        # Zephyr as UDP client
        self.target.sendline(self.linux_console, 'iperf -u -s -P 1')
        self.target.expect(self.linux_console,
                           'Server listening on UDP port 5001', timeout=180)
        # zperf udp upload <dest ip> <dest port> <duration> <packet size>
        # <bandwidth>
        self.target.sendline(
            cl_console,
            f'zperf udp upload {peer_addr} 5001 {UDP_TEST_DURATION} 1k 100K')
        self.target.expect(cl_console, r'Num packets:\s*(\d+)\s',
                           timeout=(150 * UDP_TEST_DURATION))
        self.assertGreater(int(self.target.match(cl_console)[1]), 10)
        # During this test, it can happen that error messages are shown before
        # the test ends, but the test itself is succeeding, check that no error
        # is found before the end of the test.
        self.check_error_messages(self.linux_console, cl_console)
        self.target.expect(cl_console,
                           r'Num packets out order:\s*0\r\n',
                           timeout=180)
        self.target.expect(cl_console,
                           r'Num packets lost:\s*0\r\n',
                           timeout=180)
        self.target.sendline(cl_console)
        self.target.expect(cl_console, self.si_prompt, timeout=180)
        self.target.sendline(self.linux_console)
        self.target.expect(self.linux_console, self.linux_prompt, timeout=180)
        self.check_error_messages(self.linux_console, cl_console)

        # Zephyr as TCP client
        self.target.sendline(self.linux_console, 'iperf -s -P 1')
        self.target.expect(self.linux_console,
                           'Server listening on TCP port 5001', timeout=180)
        # zperf tcp upload <dest ip> <dest port> <duration> <packet size>
        self.target.sendline(
            cl_console,
            f'zperf tcp upload {peer_addr} 5001 {TCP_TEST_DURATION} 1k')
        self.target.expect(cl_console, r'Num packets:\s*(\d+)\r\n',
                           timeout=(600 * TCP_TEST_DURATION))
        # During this test, it can happen that error messages are shown before
        # the test ends, but the test itself is succeeding, check that no error
        # is found before the end of the test.
        self.check_error_messages(self.linux_console, cl_console)
        self.assertGreater(int(self.target.match(cl_console)[1]), 10)
        self.target.expect(cl_console,
                           r'Num errors:\s*0 \(retry or fail\)',
                           timeout=225)
        self.target.sendline(cl_console)
        self.target.expect(cl_console, self.si_prompt, timeout=225)
        self.target.sendline(self.linux_console)
        self.target.expect(self.linux_console, self.linux_prompt, timeout=180)
        self.check_error_messages(self.linux_console, cl_console)

    def hipc_cluster(self, server_cl, client_cl, peer_addr):
        """
        In hipc_cluster test case, since the throughput of zperf on cluster
        depends on host performance, we only check the minimum number of
        transferred bytes(100K) to guarantee the zperf test is OK, but
        not checking the maximum throughput on the specific platform.
        """

        # Cluster as TCP client
        self.target.sendline(server_cl, f'zperf tcp download 5001 {peer_addr}')
        try:
            self.target.expect(server_cl,
                               'TCP server started on port 5001', timeout=180)
            # zperf tcp upload <dest ip> <dest port> <duration> <packet size>
            cmd = (f'zperf tcp upload {peer_addr} 5001 {TCP_TEST_DURATION} 1k '
                   '100K')
            self.target.sendline(client_cl, cmd)
            self.target.expect(client_cl, r'Num packets:\s*(\d+)\r\n',
                               timeout=(600 * TCP_TEST_DURATION))
            # During this test, it can happen that error messages are shown
            # before the test ends, but the test itself is succeeding, check
            # that no error is found before the end of the test.
            self.check_error_messages(server_cl, client_cl)
            self.assertGreater(int(self.target.match(client_cl)[1]), 10)
            self.target.expect(client_cl,
                               r'Num errors:\s*0 \(retry or fail\)',
                               timeout=225)
            self.target.sendline(client_cl)
            self.target.expect(client_cl, self.si_prompt, timeout=225)
            self.check_error_messages(server_cl, client_cl)
        finally:
            self.target.sendline(server_cl, 'zperf tcp download stop')
            self.target.expect(server_cl, 'TCP server stopped', timeout=180)

        # Cluster as UDP client
        self.target.sendline(server_cl, f'zperf udp download 5001 {peer_addr}')
        try:
            self.target.expect(server_cl,
                               'UDP server started on port 5001', timeout=180)
            # zperf udp upload <dest ip> <dest port> <duration> <packet size>
            cmd = (f'zperf udp upload {peer_addr} 5001 {UDP_TEST_DURATION} 1k '
                   '100K')
            self.target.sendline(client_cl, cmd)
            self.target.expect(client_cl,
                               r'Num packets:\s*(\d+)\s*\((\d+)\)\r\n',
                               timeout=(450 * UDP_TEST_DURATION))
            # During this test, it can happen that error messages are shown
            # before the test ends, but the test itself is succeeding, check
            # that no error is found before the end of the test.
            self.check_error_messages(server_cl, client_cl)
            self.assertGreater(int(self.target.match(client_cl)[1]), 10)
            self.target.expect(client_cl,
                               r'Num packets out order:\s*0\r\n',
                               timeout=180)
            self.target.expect(client_cl, r'Num packets lost:\s*0\r\n',
                               timeout=180)
            self.target.sendline(client_cl)
            self.target.expect(client_cl, self.si_prompt, timeout=225)
            self.check_error_messages(server_cl, client_cl)
        finally:
            self.target.sendline(server_cl, 'zperf udp download stop')
            self.target.expect(server_cl, 'UDP server stopped', timeout=180)

    @OETestDepends(['test_10_linuxlogin.LinuxLoginTest.test_linux_login'])
    def test_ping_cluster0(self):
        self.ping(r'192.168.0.1', 'safety_island_c0', r'192.168.0.2', 100)

    @OETestDepends(['test_30_hipc.HIPCTestBase.test_ping_cluster0'])
    def test_ping_cluster1(self):
        self.ping(r'192.168.1.1', 'safety_island_c1', r'192.168.1.2', 200)

    @OETestDepends(['test_30_hipc.HIPCTestBase.test_ping_cluster1'])
    def test_ping_cluster2(self):
        self.ping(r'192.168.2.1', 'safety_island_c2', r'192.168.2.2', 300)

    @OETestDepends(['test_30_hipc.HIPCTestBase.test_ping_cl1_cl2'])
    @skipIfDataVar('FREQUENCY', 'adhoc', 'Skip iperf tests in adhoc builds')
    def test_hipc_cluster0(self):
        self.hipc(r'192.168.0.1', 'safety_island_c0', r'192.168.0.2')

    @OETestDepends(['test_30_hipc.HIPCTestBase.test_hipc_cluster0'])
    @skipIfDataVar('FREQUENCY', 'adhoc', 'Skip iperf tests in adhoc builds')
    def test_hipc_cluster1(self):
        self.hipc(r'192.168.1.1', 'safety_island_c1', r'192.168.1.2')

    @OETestDepends(['test_30_hipc.HIPCTestBase.test_hipc_cluster1'])
    @skipIfDataVar('FREQUENCY', 'adhoc', 'Skip iperf tests in adhoc builds')
    def test_hipc_cluster2(self):
        self.hipc(r'192.168.2.1', 'safety_island_c2', r'192.168.2.2')

    @OETestDepends(['test_30_hipc.HIPCTestBase.test_ping_cluster2'])
    def test_ping_cl0_cl1(self):
        self.ping(r'192.168.3.1', 'safety_island_c0', r'192.168.3.2', -1)

    @OETestDepends(['test_30_hipc.HIPCTestBase.test_ping_cl0_cl1'])
    def test_ping_cl0_cl2(self):
        self.ping(r'192.168.4.1', 'safety_island_c0', r'192.168.4.2', -1)

    @OETestDepends(['test_30_hipc.HIPCTestBase.test_ping_cl0_cl2'])
    def test_ping_cl1_cl2(self):
        self.ping(r'192.168.5.1', 'safety_island_c1', r'192.168.5.2', -1)

    @OETestDepends(['test_30_hipc.HIPCTestBase.test_hipc_cluster2'])
    @skipIfDataVar('FREQUENCY', 'adhoc', 'Skip iperf tests in adhoc builds')
    def test_hipc_cluster_cl0_cl1(self):
        self.hipc_cluster('safety_island_c0', 'safety_island_c1',
                          r'192.168.3.1')

    @OETestDepends(['test_30_hipc.HIPCTestBase.test_hipc_cluster_cl0_cl1'])
    @skipIfDataVar('FREQUENCY', 'adhoc', 'Skip iperf tests in adhoc builds')
    def test_hipc_cluster_cl0_cl2(self):
        self.hipc_cluster('safety_island_c0', 'safety_island_c2',
                          r'192.168.4.1')

    @OETestDepends(['test_30_hipc.HIPCTestBase.test_hipc_cluster_cl0_cl2'])
    @skipIfDataVar('FREQUENCY', 'adhoc', 'Skip iperf tests in adhoc builds')
    def test_hipc_cluster_cl1_cl2(self):
        self.hipc_cluster('safety_island_c1', 'safety_island_c2',
                          r'192.168.5.1')

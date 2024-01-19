#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.runtime.case import OERuntimeTestCase
from oeqa.core.decorator.depends import OETestDepends
from oeqa.core.decorator.data import skipIfFeature
from oeqa.core.decorator.data import skipIfNotFeature


class ParsecTest(OERuntimeTestCase):
    def run_cmd(self, cmd, timeout):
        return self.target.run(cmd, timeout=timeout)

    @OETestDepends(['test_10_linuxboot.LinuxBootTest.test_linux_boot'])
    @skipIfFeature('baremetal',
                   'parsec-cli-tests.sh skipped for baremetal')
    def test_parsec(self):
        status, output = self.run_cmd('parsec-cli-tests.sh', timeout=1200)
        self.assertEqual(status, 0,
                         msg='Parsec CLI tests failed.\n %s' % output)
        status, output = self.run_cmd('sync', timeout=120)
        self.assertEqual(status, 0,
                         msg='Filesystem sync failed.\n %s' % output)

    @OETestDepends(['test_10_linuxboot.LinuxBootTest.test_linux_boot'])
    @skipIfNotFeature('baremetal',
                      'Test skipped due to reliance on FFA, not supported in'
                      ' virtualization')
    def test_parsec_demo(self):
        try:
            self.mirror_docker = (self.td.get('MIRROR_DOCKER') or
                                  'docker.io/library')

            # Workaround: ssh won't return if 'ssl_server &' is run, using this
            # commands below, redirecting the stderr and stdout, will make
            # ssh return but ssl_server will remain running
            cmd = 'ssl_server &>/tmp/ssl_server.log &'
            status, output = self.run_cmd(cmd, timeout=120)
            self.assertEqual(status, 0, msg='ssl_server failed to start.\n %s'
                             % output)

            status, pid = self.run_cmd('pidof ssl_server', timeout=20)
            self.assertEqual(status, 0,
                             msg='Failed to get ssl_server pid.\n %s'
                             % pid)

            status, output = self.run_cmd(f'ps -P {pid}', timeout=30)
            self.assertEqual(status, 0, msg='ssl_server is not running!.\n %s'
                             % output)

            status, output = self.run_cmd(f'docker run --rm \
                    -v /run/parsec/parsec.sock:/run/parsec/parsec.sock \
                    -v /usr/bin/ssl_client1:/usr/bin/ssl_client1 \
                    --network host \
                    {self.mirror_docker}/ubuntu:22.04 \
                    ssl_client1', timeout=800)
            self.assertEqual(status, 0, msg='ssl_client1 failed.\n %s'
                             % output)
        finally:
            # If this call fails, it means that ssl_server was not running
            # for some reason probably described in the log file
            status, output = self.run_cmd('pkill ssl_server', timeout=30)
            self.assertEqual(status, 0, msg='ssl_server failed to stop.\n %s'
                             % output)

            # Synchronize cached writes to persistent storage
            status, output = self.run_cmd('sync', timeout=120)
            self.assertEqual(status, 0, msg='Synchronizing caches failed.\n %s'
                             % output)

            status, output = self.run_cmd('cat /tmp/ssl_server.log',
                                          timeout=30)
            self.assertEqual(status, 0,
                             msg='Failed to get ssl_server logs.\n%s'
                             % output)

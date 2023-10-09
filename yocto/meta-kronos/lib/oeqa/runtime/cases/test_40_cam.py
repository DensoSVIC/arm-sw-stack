#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.runtime.case import OERuntimeTestCase
from oeqa.core.decorator.depends import OETestDepends
from oeqa.core.decorator.data import skipIfFeature
from oeqa.core.decorator.data import skipIfNotFeature


class CAMTest(OERuntimeTestCase):
    linux_console = 'default'
    hostname = r'.*'

    @classmethod
    def setUpClass(cls):
        super(CAMTest, cls).setUpClass()
        cls.linux_prompt = rf'root@{cls.hostname}:~#'

    @classmethod
    def tearDownClass(cls):
        super(CAMTest, cls).tearDownClass()

    @OETestDepends(['test_10_linuxboot.LinuxBootTest.test_linux_boot'])
    def test_cam_service_help(self):
        st = 'cam-service -h'
        status, output = self.target.run(st, timeout=20)
        self.assertEqual(status, 0, msg=f'{st} failed.\n{output}')
        self.assertTrue(r'Usage: cam-service [OPTIONS]' in output)

    @OETestDepends(['test_10_linuxboot.LinuxBootTest.test_linux_boot'])
    def test_cam_tool_help(self):
        st = 'cam-tool -h'
        status, output = self.target.run(st, timeout=60)
        self.assertEqual(status, 0, msg=f'{st} failed.\n{output}')
        self.assertTrue(r'usage: cam-tool [-h] {pack,analyze,deploy}'
                        in output)

    @OETestDepends(['test_10_linuxboot.LinuxBootTest.test_linux_boot'])
    def test_cam_app_example_help(self):
        st = 'cam-app-example -h'
        status, output = self.target.run(st, timeout=20)
        self.assertEqual(status, 0, msg=f'{st} failed.\n{output}')
        self.assertTrue(r'Usage: cam-app-example [OPTIONS]' in output)

    @OETestDepends(['test_40_cam.CAMTest.test_cam_service_help',
                    'test_40_cam.CAMTest.test_cam_tool_help',
                    'test_40_cam.CAMTest.test_cam_app_example_help'])
    def test_cam_app_example_to_service(self):
        # Check if running cam-app-example without running cam-service result
        # in failure as expected
        st = 'cam-app-example -t 3000 -c 4 -s 2'
        status, output = self.target.run(st, timeout=60)
        self.assertEqual(status, 1,
                         msg=f'Expected cam-app-example to fail.\n{output}')

        # Perform an integration test by starting cam-service and running
        # cam-app-example to see if it successfully interacts with cam-service
        st = ('cam-service -c /usr/share/cam-data/ -l info '
              '&> /tmp/cam-service.log')
        status, output = self.target.run(st, timeout=20)
        self.assertEqual(status, 0,
                         msg=f'Failed to start cam-service.\n{output}')

        st = 'cam-app-example -t 3000 -c 4 -s 2'
        status, output = self.target.run(st, timeout=60)
        self.assertEqual(status, 0,
                         msg=f'Failed to run cam-app-example.\n{output}')

        st = 'pkill -SIGINT cam-service'
        status, output = self.target.run(st, timeout=40)
        self.assertEqual(status, 0,
                         msg=f'Failed to stop cam-service.\n{output}')

        st = 'cat /tmp/cam-service.log'
        status, output = self.target.run(st, timeout=40)
        self.assertEqual(status, 0,
                         msg=f'Failed to access cam-service log.\n{output}')

    @OETestDepends(['test_40_cam.CAMTest.test_cam_app_example_to_service'])
    def test_cam_app_example_to_service_custom_uuid(self):
        # Use existing stream0.csc.yml to create a csc with custom UUID Base
        uuid_base = '99085ddc-bc10-11ed-9a44-7ef9696e'
        uuid = f'{uuid_base}0000'
        csc_f = '/usr/share/cam-data/stream0.csc.yml'
        st = f'sed -E -i \'s/uuid: "([0-9a-fA-F-]+)"/uuid: "{uuid}"/\' {csc_f}'
        status, output = self.target.run(st, timeout=20)
        self.assertEqual(status, 0,
                         msg=f'Failed to sed stream0.csc.yml\n{output}')

        status, output = self.target.run(f'cat {csc_f}', timeout=60)
        self.assertEqual(status, 0, msg=f'cat {csc_f} failed.\n{output}')

        # Use cam-tool to pack the modified stream configuration
        csd_f = f'/tmp/{uuid}.csd'
        st = f'cam-tool pack -i {csc_f} -o {csd_f}'
        status, output = self.target.run(st, timeout=60)
        self.assertEqual(status, 0, msg=f'{st} failed.\n{output}')

        # Start cam-service and then use cam-tool to deploy the new stream
        # configuration with cam-service
        st = ('cam-service -c /usr/share/cam-data/ -l info '
              '&> /tmp/cam-service.log')
        status, output = self.target.run(st, timeout=20)
        self.assertEqual(status, 0,
                         msg=f'Failed to start cam-service.\n{output}')

        st = f'cam-tool deploy -i {csd_f}'
        status, output = self.target.run(st, timeout=60)
        self.assertEqual(status, 0, msg=f'{st} failed.\n{output}')

        # Start cam-app-example with the custom uuid base
        st = f'cam-app-example -u {uuid_base} -t 3000 -c 4 -s 2'
        status, output = self.target.run(st, timeout=60)
        self.assertEqual(status, 0,
                         msg=f'Failed to run cam-app-example.\n{output}')

        st = 'pkill -SIGINT cam-service'
        status, output = self.target.run(st, timeout=40)
        self.assertEqual(status, 0,
                         msg=f'Failed to stop cam-service.\n{output}')

        # Verify that the cam-service validate the stream from cam-app-example
        # against the custom csd that was deployed.
        st = 'cat /tmp/cam-service.log'
        status, output = self.target.run(st, timeout=40)
        self.assertEqual(status, 0,
                         msg=f'Failed to access cam-service log.\n{output}')
        self.assertTrue(f'{uuid} configuration is loaded' in output)

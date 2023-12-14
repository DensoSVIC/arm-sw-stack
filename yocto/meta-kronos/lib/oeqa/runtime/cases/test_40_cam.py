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
    zephyr_console = 'safety_island_c1'
    hostname = r'.*'
    cam_data_path = '/usr/share/cam-data'
    default_uuid_base = '84085ddc-bc10-11ed-9a44-7ef9696e'
    custom_uuid_base = '99085ddc-bc10-11ed-9a44-7ef9696e'
    custom_uuid = f'{custom_uuid_base}0000'
    cam_service_si_ipaddr = '192.168.1.1'

    @classmethod
    def setUpClass(cls):
        super(CAMTest, cls).setUpClass()
        cls.linux_prompt = rf'root@{cls.hostname}:~#'

    @classmethod
    def tearDownClass(cls):
        super(CAMTest, cls).tearDownClass()

    def start_cam_service(self):
        st = (f'cam-service -c {self.cam_data_path} -l info '
              '&>/tmp/cam-service.log &')
        status, output = self.target.run(st, timeout=20)
        self.assertEqual(status, 0,
                         msg=f'Failed to start cam-service.\n{output}')

        status, pid = self.target.run('pidof cam-service', timeout=20)
        self.assertEqual(status, 0, msg='Failed to get cam-service pid.\n%s'
                         % pid)

        status, output = self.target.run(f'ps -P {pid}', timeout=30)
        self.assertEqual(status, 0, msg='cam-service is not running!.\n %s'
                         % output)

    def stop_cam_service(self):
        # If this call fails, it means that ssl_server was not running
        # for any reason
        st = 'pkill -SIGINT cam-service'
        status, output = self.target.run(st, timeout=40)
        self.assertEqual(status, 0,
                         msg=f'Failed to stop cam-service.\n{output}')

        status, output = self.target.run('cat /tmp/cam-service.log',
                                         timeout=40)
        self.assertEqual(status, 0,
                         msg=f'Failed to access cam-service log.\n{output}')
        return output

    def custom_uuid_config(self):
        uuid = f'{self.custom_uuid}'
        csc_origin = f'{self.cam_data_path}/stream0.csc.yml'
        csc_f = f'{self.cam_data_path}/custom_uuid.csc.yml'
        st = (f'sed -E \'s/uuid: "([0-9a-fA-F-]+)"/uuid: "{uuid}"/\''
              f' {csc_origin} > {csc_f}')
        status, output = self.target.run(st, timeout=20)
        self.assertEqual(status, 0,
                         msg=f'Failed to sed stream0.csc.yml\n{output}')

        status, output = self.target.run(f'cat {csc_f}', timeout=60)
        self.assertEqual(status, 0, msg=f'cat {csc_f} failed.\n{output}')

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
        self.assertTrue(r'usage: cam-tool [-h] {analyze,pack,deploy}'
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
    def test_cam_app_example_to_service_on_pc(self):
        # Check if running cam-app-example without running cam-service result
        # in failure as expected
        st = 'cam-app-example -t 3000 -c 4 -s 2'
        status, output = self.target.run(st, timeout=60)
        self.assertEqual(status, 1,
                         msg=f'Expected cam-app-example to fail.\n{output}')

        # Perform an integration test by starting cam-service and running
        # cam-app-example to see if it successfully interacts with cam-service
        try:
            self.start_cam_service()

            st = 'cam-app-example -t 3000 -c 4 -s 2'
            status, output = self.target.run(st, timeout=200)
            self.assertEqual(status, 0,
                             msg=f'Failed to run cam-app-example.\n{output}')
        finally:
            self.stop_cam_service()

    @OETestDepends(['test_40_cam.CAMTest.test_cam_app_example_to_service_on_pc'])
    def test_cam_tool_pack(self):
        self.custom_uuid_config()

        uuid = f'{self.custom_uuid}'
        csc_f = f'{self.cam_data_path}/custom_uuid.csc.yml'

        # Use cam-tool to pack the modified stream configuration
        csd_f = f'/tmp/{uuid}.csd'
        st = f'cam-tool pack -i {csc_f} -o {csd_f}'
        status, output = self.target.run(st, timeout=180)
        self.assertEqual(status, 0, msg=f'{st} failed.\n{output}')

    @OETestDepends(['test_40_cam.CAMTest.test_cam_tool_pack'])
    def test_cam_app_example_with_custom_uuid_to_service_on_pc(self):
        uuid = f'{self.custom_uuid}'
        csd_f = f'/tmp/{uuid}.csd'

        # Start cam-service and then use cam-tool to deploy the new stream
        # configuration with cam-service
        try:
            self.start_cam_service()

            st = f'cam-tool deploy -i {csd_f} -o'
            status, output = self.target.run(st, timeout=180)
            self.assertEqual(status, 0, msg=f'{st} failed.\n{output}')

            # Start cam-app-example with the custom uuid base
            st = (f'cam-app-example -u {self.custom_uuid_base} -t 3000'
                  ' -c 4 -s 2')
            status, output = self.target.run(st, timeout=200)
            self.assertEqual(status, 0,
                             msg=f'Failed to run cam-app-example.\n{output}')
        finally:
            output = self.stop_cam_service()
            self.assertTrue(f'{uuid} configuration is loaded' in output)

    @OETestDepends([
        'test_40_cam.CAMTest.test_cam_app_example_with_custom_uuid_to_service_on_pc'])
    def test_data_calibration_on_pc(self):
        uuid_base = self.default_uuid_base
        csc_file_name = 'calibration_generate'

        st = ('cam-app-example --enable-calibration-mode'
              f' --calibration-directory={self.cam_data_path}')
        self.target.sendline(self.linux_console, st)
        self.target.expect(self.linux_console, self.linux_prompt, timeout=60)

        st = f'ls {self.cam_data_path}/{uuid_base}0000.csel'
        self.target.sendline(self.linux_console, st)
        self.target.expect(self.linux_console,
                           f'{self.cam_data_path}/{uuid_base}0000.csel',
                           timeout=60)
        self.target.expect(self.linux_console, self.linux_prompt, timeout=60)

        st = ('cam-tool analyze'
              f' -i {self.cam_data_path}/{uuid_base}0000.csel'
              f' -o {self.cam_data_path}/{csc_file_name}.csc.yml')
        self.target.sendline(self.linux_console, st)
        self.target.expect(self.linux_console, self.linux_prompt, timeout=180)

        st = f'ls {self.cam_data_path}/{csc_file_name}.csc.yml'
        self.target.sendline(self.linux_console, st)
        self.target.expect(self.linux_console,
                           f'{self.cam_data_path}/{csc_file_name}.csc.yml',
                           timeout=60)

        self.target.expect(self.linux_console, self.linux_prompt, timeout=60)

    def test_cam_service_boot_on_si(self):
        self.target.expect(self.zephyr_console,
                           r'Cam service configuration:',
                           timeout=180)
        self.target.expect(self.zephyr_console, 'uart:~\$', timeout=180)

    @OETestDepends([
        'test_40_cam.CAMTest.test_cam_service_boot_on_si',
        'test_40_cam.CAMTest.test_data_calibration_on_pc'])
    def test_cam_tool_deploy_to_si(self):
        for i in range(4):
            # Deploy deployment files to Safety Island
            st = ('cam-tool deploy -i'
                  f' {self.cam_data_path}/{self.default_uuid_base}000{i}.csd'
                  f' -a {self.cam_service_si_ipaddr} -o')
            self.target.sendline(self.linux_console, st)
            self.target.expect(self.linux_console,
                               self.linux_prompt,
                               timeout=180)

            # Verify whether the file exists
            st = f'fs read /RAM:/{self.default_uuid_base}000{i}.csd'
            self.target.sendline(self.zephyr_console, st)
            self.target.expect(self.zephyr_console,
                               r'File size: 104',
                               timeout=30)

    @OETestDepends(['test_40_cam.CAMTest.test_cam_tool_deploy_to_si'])
    def test_cam_app_example_to_service_on_si(self):
        processing_count = 4
        stream_count = 4

        st = (f'cam-app-example -a {self.cam_service_si_ipaddr}'
              f' --processing-count {processing_count}'
              f' --stream-count {stream_count}')
        self.target.sendline(self.linux_console, st)
        for _ in range((processing_count + 2) * stream_count):
            self.target.expect(self.zephyr_console,
                               'Start|Event|Stop',
                               timeout=60)

        self.target.expect(self.linux_console, self.linux_prompt, timeout=180)

    @OETestDepends([
        'test_40_cam.CAMTest.test_cam_app_example_to_service_on_si'])
    def test_cam_app_example_to_service_on_si_with_multiple_connection(self):
        processing_count = 4
        stream_count = 4

        st = (f'cam-app-example -a {self.cam_service_si_ipaddr}'
              f' --processing-count {processing_count}'
              f' --stream-count {stream_count}'
              ' --enable-multiple-connection')
        self.target.sendline(self.linux_console, st)
        for _ in range((processing_count + 2) * stream_count):
            self.target.expect(self.zephyr_console,
                               'Start|Event|Stop',
                               timeout=300)

        self.target.expect(self.linux_console, self.linux_prompt, timeout=180)

    @OETestDepends([
        'test_40_cam.CAMTest.test_cam_app_example_to_service_on_si_with_multiple_connection'])
    def test_logical_check_on_si(self):
        event_interval = "0,100"

        st = (f'cam-app-example -a {self.cam_service_si_ipaddr}'
              f' --event-interval={event_interval}')
        self.target.sendline(self.linux_console, st)
        self.target.expect(self.zephyr_console,
                           r'Stream logical error',
                           timeout=300)
        self.target.expect(self.linux_console, self.linux_prompt, timeout=180)

    @OETestDepends([
        'test_40_cam.CAMTest.test_logical_check_on_si'])
    def test_temporal_check_on_si(self):
        st = (f'cam-app-example -a {self.cam_service_si_ipaddr}'
              ' --enable-fault-injection'
              ' --fault-injection-time=8000'
              ' --processing-count=4')
        self.target.sendline(self.linux_console, st)
        self.target.expect(self.zephyr_console,
                           r'Stream temporal error',
                           timeout=300)
        self.target.expect(self.linux_console, self.linux_prompt, timeout=180)

#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

import re
from oeqa.runtime.case import OERuntimeTestCase
from oeqa.core.decorator.depends import OETestDepends
from oeqa.utils.linux_terminal_utils import LinuxTermUtils
from oeqa.utils.zephyr_shell import Shell


class CAMTest(OERuntimeTestCase):
    zephyr_console = 'safety_island_c1'
    hostname = r'fvp-rd-kronos'
    cam_data_path = '/usr/share/cam-data'
    default_uuid_base = '84085ddc-bc10-11ed-9a44-7ef9696e'
    custom_uuid_base = '99085ddc-bc10-11ed-9a44-7ef9696e'
    custom_uuid = f'{custom_uuid_base}0000'
    cam_service_si_ipaddr = '192.168.1.1'

    @classmethod
    def setUpClass(cls):
        super(CAMTest, cls).setUpClass()
        cls.linux_prompt = rf'root@{cls.hostname}:~#'
        linux_console = cls.tc.target._get_terminal('default')
        cls.lt_utils = LinuxTermUtils(cls.tc, linux_console, cls.linux_prompt)
        cls.si1_shell = Shell(cls.tc.target, cls.zephyr_console, cls.tc.logger)

    @classmethod
    def tearDownClass(cls):
        super(CAMTest, cls).tearDownClass()

    def cam_service_ctx(self):
        bg_cmd = f'cam-service -c {self.cam_data_path} -l info'
        return self.lt_utils.background_cmd_ctx(bg_cmd)

    def start_cam_app(self, uuid_base=None, stream_count=2):
        cmd = f'cam-app-example -t 3000 -c 4 -s {stream_count}'
        if uuid_base is not None:
            cmd += f' -u {uuid_base}'

        return self.lt_utils.run(cmd, timeout=180)

    def custom_uuid_config(self):
        uuid = f'{self.custom_uuid}'
        csc_origin = f'{self.cam_data_path}/stream0.csc.yml'
        csc_f = f'{self.cam_data_path}/custom_uuid.csc.yml'
        st = (f'sed -E \'s/uuid: "([0-9a-fA-F-]+)"/uuid: "{uuid}"/\''
              f' {csc_origin} > {csc_f}')
        status, output = self.lt_utils.run(st, timeout=20)
        self.assertEqual(status, 0,
                         msg=f'Failed to sed stream0.csc.yml\n{output}')

        status, output = self.lt_utils.run(f'cat {csc_f}', timeout=60)
        self.assertEqual(status, 0, msg=f'cat {csc_f} failed.\n{output}')

    @OETestDepends(['test_10_linuxboot.LinuxBootTest.test_linux_boot'])
    def test_cam_service_help(self):
        st = 'cam-service -h'
        status, output = self.lt_utils.run(st, timeout=20)
        self.assertEqual(status, 0, msg=f'{st} failed.\n{output}')
        self.assertTrue(r'Usage: cam-service [OPTIONS]' in output)

    @OETestDepends(['test_10_linuxboot.LinuxBootTest.test_linux_boot'])
    def test_cam_tool_help(self):
        st = 'cam-tool -h'
        status, output = self.lt_utils.run(st, timeout=60)
        self.assertEqual(status, 0, msg=f'{st} failed.\n{output}')
        self.assertTrue(r'usage: cam-tool [-h] {analyze,pack,deploy}'
                        in output)

    @OETestDepends(['test_10_linuxboot.LinuxBootTest.test_linux_boot'])
    def test_cam_app_example_help(self):
        st = 'cam-app-example -h'
        status, output = self.lt_utils.run(st, timeout=20)
        self.assertEqual(status, 0, msg=f'{st} failed.\n{output}')
        self.assertTrue(r'Usage: cam-app-example [OPTIONS]' in output)

    @OETestDepends(['test_40_cam.CAMTest.test_cam_service_help',
                    'test_40_cam.CAMTest.test_cam_tool_help',
                    'test_40_cam.CAMTest.test_cam_app_example_help'])
    def test_cam_app_example_to_service_on_pc(self):
        # Check if running cam-app-example without running cam-service result
        # in failure as expected
        status, output = self.start_cam_app()
        self.assertNotEqual(status, 0,
                            msg=f'Expected cam-app-example to fail.\n{output}')

        # Perform an integration test by starting cam-service and running
        # cam-app-example to see if it successfully interacts with cam-service
        with self.cam_service_ctx():
            status, _ = self.start_cam_app()
            self.assertEqual(status, 0,
                             msg=f'Failed to run cam-app-example.')

    @OETestDepends(['test_40_cam.CAMTest.test_cam_app_example_to_service_on_pc'])
    def test_cam_tool_pack(self):
        self.custom_uuid_config()

        uuid = f'{self.custom_uuid}'
        csc_f = f'{self.cam_data_path}/custom_uuid.csc.yml'

        # Use cam-tool to pack the modified stream configuration
        csd_f = f'/tmp/{uuid}.csd'
        st = f'cam-tool pack -i {csc_f} -o {csd_f}'
        status, output = self.lt_utils.run(st, timeout=180)
        self.assertEqual(status, 0, msg=f'{st} failed.\n{output}')

    @OETestDepends(['test_40_cam.CAMTest.test_cam_tool_pack'])
    def test_cam_app_example_with_custom_uuid_to_service_on_pc(self):
        csd_f = f'/tmp/{self.custom_uuid}.csd'

        # Start cam-service and then use cam-tool to deploy the new stream
        # configuration with cam-service
        cam_serv_ctx = self.cam_service_ctx()
        with cam_serv_ctx:
            st = f'cam-tool deploy -i {csd_f} -o'
            status, output = self.lt_utils.run(st, timeout=180)
            self.assertEqual(status, 0, msg=f'{st} failed.\n{output}')

            status, _ = self.start_cam_app(uuid_base=self.custom_uuid_base,
                                           stream_count=1)
            self.assertEqual(status, 0,
                             msg=f'Failed to run cam-app-example.')

        self.assertIn(f'{uuid} configuration is loaded',
                      cam_serv_ctx.cmd_output,
                      f'Failed! {uuid} not found in cam-service configuration!')

    @OETestDepends([
        'test_40_cam.CAMTest.test_cam_app_example_with_custom_uuid_to_service_on_pc'])
    def test_data_calibration_on_pc(self):
        uuid_base = self.default_uuid_base
        csc_file = f'{self.cam_data_path}/calibration_generate.csc.yml'

        st = ('cam-app-example --enable-calibration-mode'
              f' --calibration-directory={self.cam_data_path}')
        status, _ = self.lt_utils.run(st)
        self.assertEqual(status, 0,
                         msg='Failed to run cam-app-example calibration mode.')

        calib_file = f"{self.cam_data_path}/{uuid_base}0000.csel"
        st = f"test -f {calib_file}"
        status, _ = self.lt_utils.run(st)
        self.assertEqual(status, 0,
                         msg=f'Failed to fetch {calib_file}')

        st = (f'cam-tool analyze -i {calib_file} -o {csc_file}')
        status, _ = self.lt_utils.run(st, timeout=180)
        self.assertEqual(status, 0,
                         msg=f'An error has occurred for cam-tool')

        st = f'test -f {csc_file}'
        status, _ = self.lt_utils.run(st)
        self.assertEqual(status, 0,
                         msg=f'Failed to fetch {csc_file}')

    def test_cam_service_boot_on_si(self):
        self.target.expect(self.zephyr_console,
                           r'Cam service configuration:',
                           timeout=180)
        self.target.expect(self.zephyr_console, r'uart:~\$', timeout=180)

    @OETestDepends([
        'test_40_cam.CAMTest.test_cam_service_boot_on_si',
        'test_40_cam.CAMTest.test_data_calibration_on_pc'])
    def test_cam_tool_deploy_to_si(self):
        for i in range(4):
            # Deploy deployment files to Safety Island
            csd = f"{self.cam_data_path}/{self.default_uuid_base}000{i}.csd"
            st = (f'cam-tool deploy -i {csd}'
                  f' -a {self.cam_service_si_ipaddr} -o')
            status, _ = self.lt_utils.run(st)
            self.assertEqual(status, 0,
                             msg=f'cam-tool failed to deploy {csd}')

            # Verify whether the file exists
            st = f'fs read /RAM:/{self.default_uuid_base}000{i}.csd'
            output = self.si1_shell.exec_command(st, timeout=60)
            self.assertIn('File size: 104', output,
                          ('SI: Configuration error for '
                           f'/RAM:/{self.default_uuid_base}000{i}.csd'))

    def check_si_streams(self, stream_count, processing_count):
        self.target.sendline(self.zephyr_console)
        self.target.expect(self.zephyr_console, r'uart:~\$', timeout=30)
        si_output = self.target.before(self.zephyr_console)
        si_output = si_output.decode("utf-8", errors="replace").strip()

        for i in range(stream_count):
            msg = (f'Stream {self.default_uuid_base}000{i} configuration is '
                   'loaded.')
            self.assertIn(msg, si_output,
                          f'Stream {self.default_uuid_base}000{i} not loaded!')
        for msg in ['Init', 'Start', 'Stop']:
            matches = re.findall(rf'({msg} Message)', si_output)
            self.assertEqual(len(matches), stream_count,
                             msg=(f'{msg} Message count doesn\'t match stream '
                                  f'count ({stream_count})'))
        matches = re.findall(r'(Event Message)', si_output)
        self.assertEqual(len(matches), stream_count * processing_count,
                         msg=('Event Message count doesn\'t match expected '
                              f'count ({stream_count * processing_count})'))

    @OETestDepends(['test_40_cam.CAMTest.test_cam_tool_deploy_to_si'])
    def test_cam_app_example_to_service_on_si(self):
        processing_count = 4
        stream_count = 4

        st = (f'cam-app-example -a {self.cam_service_si_ipaddr}'
              f' --processing-count {processing_count}'
              f' --stream-count {stream_count}')
        self.si1_shell.wait_for_prompt()
        status, _ = self.lt_utils.run(st, timeout=60*stream_count)
        self.assertEqual(status, 0, msg='cam-app-example failed.')
        self.check_si_streams(stream_count, processing_count)

    @OETestDepends([
        'test_40_cam.CAMTest.test_cam_app_example_to_service_on_si'])
    def test_cam_app_example_to_service_on_si_with_multiple_connection(self):
        processing_count = 4
        stream_count = 4

        st = (f'cam-app-example -a {self.cam_service_si_ipaddr}'
              f' --processing-count {processing_count}'
              f' --stream-count {stream_count}'
              ' --enable-multiple-connection')
        self.si1_shell.wait_for_prompt()
        status, _ = self.lt_utils.run(st, timeout=60*stream_count)
        self.assertEqual(status, 0, msg='cam-app-example failed.')
        self.check_si_streams(stream_count, processing_count)

    @OETestDepends([
        'test_40_cam.CAMTest.test_cam_app_example_to_service_on_si_with_multiple_connection'])
    def test_logical_check_on_si(self):
        event_interval = "0,100"

        st = (f'cam-app-example -a {self.cam_service_si_ipaddr}'
              f' --event-interval={event_interval}')
        status, _ = self.lt_utils.run(st)
        self.assertEqual(status, 0,
                         msg='cam-app-example failed.')
        self.target.expect(self.zephyr_console, r'Stream logical error',
                           timeout=300)

    @OETestDepends([
        'test_40_cam.CAMTest.test_logical_check_on_si'])
    def test_temporal_check_on_si(self):
        st = (f'cam-app-example -a {self.cam_service_si_ipaddr}'
              ' --enable-fault-injection'
              ' --fault-injection-time=8000'
              ' --processing-count=4')
        status, _ = self.lt_utils.run(st)
        self.assertEqual(status, 0,
                         msg='cam-app-example failed.')
        self.target.expect(self.zephyr_console, r'Stream temporal error',
                           timeout=300)

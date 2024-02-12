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
from oeqa.utils.xen_utils import XenUtils


class CAMTest(OERuntimeTestCase):
    zephyr_console = 'safety_island_c1'
    hostname = r'fvp-rd-kronos'
    domu_hostname = r'domu1'
    uuid_base_a = '11085ddc-bc10-11ed-9a44-7ef9696e'
    streams_a = 4
    uuid_base_b = '22085ddc-bc10-11ed-9a44-7ef9696e'
    streams_b = 2
    processing_count = 4
    cam_service_si_ipaddr = '192.168.1.1'

    @classmethod
    def setUpClass(cls):
        super(CAMTest, cls).setUpClass()
        cls.linux_prompt = rf'root@{cls.hostname}:~#'
        linux_console = cls.tc.target._get_terminal('default')
        cls.si1_shell = Shell(cls.tc.target, cls.zephyr_console, cls.tc.logger)
        if ('virtualization' in cls.td.get('IMAGE_FEATURES', '').split()):
            cls.linux_prompt = rf'root@{cls.domu_hostname}:~#'
            cls.dom0_prompt = rf'root@{cls.hostname}:~#'
            linux_console = LinuxTermUtils.open_ssh_shell(cls.tc.target,
                                                          cls.domu_hostname,
                                                          cls.tc.logger)
            XenUtils.enter_guest_from_dom0(linux_console, cls.dom0_prompt,
                                           cls.linux_prompt, cls.domu_hostname)
        cls.lt_utils = LinuxTermUtils(cls.tc, linux_console, cls.linux_prompt)

        cls.uuids = (
            [f"{cls.uuid_base_a}{n:04}" for n in range(cls.streams_a)] +
            [f"{cls.uuid_base_b}{n:04}" for n in range(cls.streams_b)]
        )

    @classmethod
    def tearDownClass(cls):
        if ('virtualization' in cls.td.get('IMAGE_FEATURES', '').split()):
            XenUtils.exit_guest_to_dom0(cls.lt_utils.console, cls.dom0_prompt,
                                        cls.linux_prompt, cls.domu_hostname,
                                        False)
            LinuxTermUtils.close_ssh_shell(cls.lt_utils.console, cls.tc.logger)
        super(CAMTest, cls).tearDownClass()

    def start_cam_app(self, uuid_base=None, stream_count=2):
        cmd = f'cam-app-example -t 3000 -c 4 -s {stream_count}'
        if uuid_base is not None:
            cmd += f' -u {uuid_base}'

        return self.lt_utils.run(cmd, timeout=180)

    @OETestDepends(['test_10_linuxboot.LinuxBootTest.test_linux_boot'])
    def test_data_calibration(self):
        for uuid_base, streams in (
            (self.uuid_base_a, self.streams_a),
            (self.uuid_base_b, self.streams_b)
        ):
            st = (f'cam-app-example -u {uuid_base} --enable-calibration-mode'
                  f' -s {streams}')
            status, output = self.lt_utils.run(st)
            self.tc.logger.debug(output)
            self.assertEqual(
                status, 0,
                msg='Failed to run cam-app-example calibration mode.'
            )

        for uuid in self.uuids:
            csc_file = f'{uuid}.csc.yml'
            calib_file = f"{uuid}.csel"
            st = f"test -f {calib_file}"
            status, _ = self.lt_utils.run(st)
            self.assertEqual(status, 0,
                             msg=f'Failed to fetch {calib_file}')

            st = (f'cam-tool analyze -m 1000000 -i {calib_file}')

            status, _ = self.lt_utils.run(st, timeout=180)
            self.assertEqual(status, 0,
                             msg=f'An error has occurred for cam-tool')

            st = f'test -f {csc_file}'
            status, _ = self.lt_utils.run(st)
            self.assertEqual(status, 0,
                             msg=f'Failed to fetch {csc_file}')

    @OETestDepends(['test_40_cam.CAMTest.test_data_calibration'])
    def test_cam_tool_pack(self):
        for uuid in self.uuids:
            csc_f = f'{uuid}.csc.yml'

            # Use cam-tool to pack the modified stream configuration
            st = f'cam-tool pack -i {csc_f}'
            status, output = self.lt_utils.run(st, timeout=180)
            self.assertEqual(status, 0, msg=f'{st} failed.\n{output}')

    def test_cam_service_boot_on_si(self):
        self.target.expect(self.zephyr_console,
                           r'Cam service configuration:',
                           timeout=180)
        self.target.expect(self.zephyr_console, r'uart:~\$', timeout=180)

    @OETestDepends([
        'test_40_cam.CAMTest.test_cam_service_boot_on_si',
        'test_40_cam.CAMTest.test_cam_tool_pack'])
    def test_cam_tool_deploy_to_si(self):
        for uuid in self.uuids:
            # Deploy deployment files to Safety Island
            csd = f"{uuid}.csd"
            st = (f'cam-tool deploy -i {csd}'
                  f' -a {self.cam_service_si_ipaddr} -o')
            status, _ = self.lt_utils.run(st)
            self.assertEqual(status, 0,
                             msg=f'cam-tool failed to deploy {csd}')

            # Verify whether the file exists
            st = f'fs read /RAM:/{uuid}.csd'
            output = self.si1_shell.exec_command(st, timeout=60)
            self.assertIn('File size: 104', output,
                          ('SI: Configuration error for '
                           f'/RAM:/{uuid}.csd'))

    def run_check_errors(self, cmd, timeout):
        def run(cmd, timeout):
            lt_run_return = self.lt_utils.run(cmd=cmd, timeout=timeout)
            self.si1_shell.send_empty_line()

            return lt_run_return

        lines, fn_return = self.si1_shell.exec_fn(
            run, cmd=cmd, timeout=timeout)

        self.assertFalse("ERROR:" in lines, "Errors found on cam-service.")

        return fn_return

    @OETestDepends(['test_40_cam.CAMTest.test_cam_tool_deploy_to_si'])
    def test_cam_app_example_to_service_on_si(self):
        st = (f'cam-app-example -u {self.uuid_base_a}'
              f' -a {self.cam_service_si_ipaddr}'
              f' --processing-count {self.processing_count}'
              f' --stream-count {self.streams_a}')
        status, _ = self.run_check_errors(st, timeout=60*self.streams_a)
        self.assertEqual(status, 0, msg='cam-app-example failed.')

    @OETestDepends([
        'test_40_cam.CAMTest.test_cam_app_example_to_service_on_si'])
    def test_cam_app_example_to_service_on_si_with_multiple_connections(self):
        st = (f'cam-app-example -u {self.uuid_base_a}'
              f' -a {self.cam_service_si_ipaddr}'
              f' --processing-count {self.processing_count}'
              f' --stream-count {self.streams_a}'
              ' --enable-multiple-connection')
        status, _ = self.run_check_errors(st, timeout=60*self.streams_a)
        self.assertEqual(status, 0, msg='cam-app-example failed.')

    @OETestDepends([
        'test_40_cam.CAMTest.test_cam_app_example_to_service'
        '_on_si_with_multiple_connections'])
    def test_logical_check_on_si(self):
        event_interval = "0,100"

        st = (f'cam-app-example -u {self.uuid_base_a}'
              f' -a {self.cam_service_si_ipaddr}'
              f' --event-interval={event_interval}')
        status, _ = self.lt_utils.run(st)
        self.assertEqual(status, 0,
                         msg='cam-app-example failed.')
        self.target.expect(self.zephyr_console, r'Stream logical error',
                           timeout=300)

    @OETestDepends([
        'test_40_cam.CAMTest.test_logical_check_on_si'])
    def test_temporal_check_on_si(self):
        st = (f'cam-app-example -u {self.uuid_base_a}'
              f' -a {self.cam_service_si_ipaddr}'
              ' --enable-fault-injection'
              ' --fault-injection-time=8000'
              f' --processing-count={self.processing_count}')
        status, _ = self.lt_utils.run(st, timeout=120)
        self.assertEqual(status, 0,
                         msg='cam-app-example failed.')
        self.target.expect(self.zephyr_console, r'Stream temporal error',
                           timeout=300)

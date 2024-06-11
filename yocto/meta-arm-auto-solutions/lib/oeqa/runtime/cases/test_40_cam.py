#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

import re
from oeqa.runtime.case import OERuntimeTestCase
from oeqa.core.decorator.depends import OETestDepends
from oeqa.core.decorator.data import skipIfFeature
from oeqa.utils.linux_terminal_utils import LinuxTermUtils, LinuxMultiTermUtils
from oeqa.utils.zephyr_shell import Shell
from oeqa.utils.xen_utils import XenUtils
from oeqa.utils.arm_auto_solutions_config import ArmAutoSolutionsConfig
from time import sleep
import unittest


###############################################################################
# Global variables and console helpers                                        #
###############################################################################


class Baremetal:
    uuid_base = '11085ddc-bc10-11ed-9a44-7ef9696e'
    streams = 4
    processing_count = 4

    def __init__(self):
        self.uuids = [f"{self.uuid_base}{n:04}" for n in range(self.streams)]


class DomU1(Baremetal):
    domu_hostname = ArmAutoSolutionsConfig.domu1_hostname

    def __init__(self, tc):
        self.linux_prompt = rf'root@{self.domu_hostname}:~#'
        self.tc = tc
        linux_console = XenUtils.spawn_console_domu(
            self.linux_prompt,
            ArmAutoSolutionsConfig.dom0_prompt,
            self.domu_hostname,
            self.tc.target,
            self.tc.logger)
        self.lt_utils = LinuxTermUtils(self.tc, linux_console,
                                       self.linux_prompt)

        super().__init__()

    def get_lt_utils(self):
        return self.lt_utils

    def shut_down(self):
        XenUtils.close_console_domu(self.lt_utils, self.linux_prompt,
                                    ArmAutoSolutionsConfig.dom0_prompt,
                                    self.domu_hostname, self.tc.logger)


class DomU2(DomU1):
    domu_hostname = ArmAutoSolutionsConfig.domu2_hostname
    uuid_base = '22085ddc-bc10-11ed-9a44-7ef9696e'
    streams = 2
    processing_count = 4


###############################################################################
# Test suites                                                                 #
###############################################################################


class CAMServiceTest(OERuntimeTestCase):
    def test_cam_service_boot_on_si(self):
        self.target.expect(ArmAutoSolutionsConfig.si_cl1_console_name,
                           r'Cam service configuration:',
                           timeout=270)
        self.target.expect(
            ArmAutoSolutionsConfig.si_cl1_console_name, r'uart:~\$',
            timeout=270)


class CAMTest(OERuntimeTestCase):
    """
    Run tests on either Dom0 or DomU1 for the baremetal or virtualization cases
    respectively.
    """

    @classmethod
    def setUpClass(cls):
        super(CAMTest, cls).setUpClass()
        cls.si1_shell = Shell(
            cls.tc.target,
            ArmAutoSolutionsConfig.si_cl1_console_name, cls.tc.logger)

        if not ('virtualization' in cls.td.get('IMAGE_FEATURES', '').split()):
            cls.linux_prompt = ArmAutoSolutionsConfig.baremetal_prompt
            linux_console = cls.tc.target._get_terminal('default')
            cls.lt_utils = LinuxTermUtils(cls.tc, linux_console,
                                          cls.linux_prompt)
            cls.dom = Baremetal()
        else:
            cls.dom = DomU1(cls.tc)
            cls.lt_utils = cls.dom.get_lt_utils()

    @classmethod
    def tearDownClass(cls):
        if ('virtualization' in cls.td.get('IMAGE_FEATURES', '').split()):
            cls.dom.shut_down()

        super(CAMTest, cls).tearDownClass()

    @OETestDepends(['test_10_linuxlogin.LinuxLoginTest.test_linux_login'])
    def test_cam_ptp_sync(self):
        # The current setup is that the DomUs clock need to be in sync with the
        # clock from Dom0, in the baremetal case we don't need to do anything.
        # This test is not using any skipIf(Not)Feature decorator because it
        # will be a dependency of the following tests, so we can't skip it as
        # it would stop the others from running
        if 'virtualization' not in self.td.get('IMAGE_FEATURES', '').split():
            return

        retry = 3
        expected_string = 'NTPSynchronized=yes'
        while retry > 0:
            status, output = self.lt_utils.run('timedatectl show')
            self.assertEqual(status, 0, msg=f'timedatectl command failed')
            if expected_string in output:
                break
            retry -= 1
            sleep(60)

        # This command is here only to debug cases when the clocks are not
        # in sync
        if expected_string not in output:
            cmd = 'systemctl status ptp4l@ethsi1 -l'
            status, _ = self.lt_utils.run(cmd)
            self.assertEqual(status, 0,
                             msg=f'systemctl status ptp4l[...] failed')

        self.assertIn(expected_string, output,
                      msg='PTP clocks are not in sync.')

    @OETestDepends([
        'test_40_cam.CAMServiceTest.test_cam_service_boot_on_si',
        'test_40_cam.CAMTest.test_cam_tool_pack'])
    def test_cam_tool_deploy_to_si(self):
        for uuid in self.dom.uuids:
            # Deploy deployment files to Safety Island
            csd = f"{uuid}.csd"
            st = (f'cam-tool deploy -i {csd}'
                  f' -a {ArmAutoSolutionsConfig.si_cl1_ipaddr} -o')
            status, _ = self.lt_utils.run(st)
            self.assertEqual(status, 0,
                             msg=f'cam-tool failed to deploy {csd}')

            # Verify whether the file exists
            st = f'fs read /RAM:/{uuid}.csd'
            output = self.si1_shell.exec_command(st, timeout=90)
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
        st = (f'cam-app-example -u {self.dom.uuid_base}'
              f' -a {ArmAutoSolutionsConfig.si_cl1_ipaddr}'
              f' --processing-count {self.dom.processing_count}'
              f' --stream-count {self.dom.streams}')
        status, _ = self.run_check_errors(st, timeout=90*self.dom.streams)
        self.assertEqual(status, 0, msg='cam-app-example failed.')

    @OETestDepends([
        'test_40_cam.CAMTest.test_cam_app_example_to_service_on_si'])
    def test_cam_app_example_to_service_on_si_with_multiple_connections(self):
        st = (f'cam-app-example -u {self.dom.uuid_base}'
              f' -a {ArmAutoSolutionsConfig.si_cl1_ipaddr}'
              f' --processing-count {self.dom.processing_count}'
              f' --stream-count {self.dom.streams}'
              ' --enable-multiple-connection')
        status, _ = self.run_check_errors(st, timeout=90*self.dom.streams)
        self.assertEqual(status, 0, msg='cam-app-example failed.')

    @OETestDepends([
        'test_40_cam.CAMTest.'
        'test_cam_app_example_to_service_on_si_with_multiple_connections'])
    def test_logical_check_on_si(self):
        event_interval = "0,100"

        st = (f'cam-app-example -u {self.dom.uuid_base}'
              f' -a {ArmAutoSolutionsConfig.si_cl1_ipaddr}'
              f' --event-interval={event_interval}')
        status, _ = self.lt_utils.run(st)
        self.assertEqual(status, 0,
                         msg='cam-app-example failed.')
        self.target.expect(ArmAutoSolutionsConfig.si_cl1_console_name,
                           r'Stream logical error',
                           timeout=450)

    @OETestDepends(['test_40_cam.CAMTest.test_logical_check_on_si'])
    def test_temporal_check_on_si(self):
        st = (f'cam-app-example -u {self.dom.uuid_base}'
              f' -a {ArmAutoSolutionsConfig.si_cl1_ipaddr}'
              ' --enable-fault-injection'
              ' --fault-injection-time=8000'
              f' --processing-count={self.dom.processing_count}')
        status, _ = self.lt_utils.run(st, timeout=180)
        self.assertEqual(status, 0,
                         msg='cam-app-example failed.')
        self.target.expect(ArmAutoSolutionsConfig.si_cl1_console_name,
                           r'Stream temporal error',
                           timeout=450)

    @OETestDepends(['test_40_cam.CAMTest.test_cam_ptp_sync'])
    def test_data_calibration(self):
        st = (f'cam-app-example -u {self.dom.uuid_base} '
              f' --enable-calibration-mode -s {self.dom.streams}')
        status, output = self.lt_utils.run(st, timeout=180)
        self.tc.logger.debug(output)
        self.assertEqual(
            status, 0,
            msg='Failed to run cam-app-example calibration mode.'
        )
        init_to_start = 700000
        start_to_event = 700000

        for uuid in self.dom.uuids:
            csc_file = f'{uuid}.csc.yml'
            calib_file = f"{uuid}.csel"
            st = f"test -f {calib_file}"
            status, _ = self.lt_utils.run(st)
            self.assertEqual(status, 0,
                             msg=f'Failed to fetch {calib_file}')

            st = (f'cam-tool analyze -m 1000000 -i {calib_file}')

            status, _ = self.lt_utils.run(st, timeout=270)
            self.assertEqual(status, 0,
                             msg=f'An error has occurred for cam-tool')

            st = f'test -f {csc_file}'
            status, _ = self.lt_utils.run(st)
            self.assertEqual(status, 0,
                             msg=f'Failed to fetch {csc_file}')

            st = (f'sed -i -E \'s/timeout_init_to_start: ([0-9]+)'
                  f'/timeout_init_to_start: {init_to_start}/\' {csc_file}')
            status, output = self.lt_utils.run(st, timeout=30)
            self.assertEqual(status, 0,
                             msg=f'Failed to sed {csc_file}\n{output}')

            st = (f'sed -i -E \'s/timeout_start_to_event: ([0-9]+)'
                  f'/timeout_start_to_event: {start_to_event}/\' {csc_file}')
            status, output = self.lt_utils.run(st, timeout=30)
            self.assertEqual(status, 0,
                             msg=f'Failed to sed {csc_file}\n{output}')

    @OETestDepends(['test_40_cam.CAMTest.test_data_calibration'])
    def test_cam_tool_pack(self):
        for uuid in self.dom.uuids:
            csc_f = f'{uuid}.csc.yml'

            # Use cam-tool to pack the modified stream configuration
            st = f'cam-tool pack -i {csc_f}'
            status, output = self.lt_utils.run(st, timeout=270)
            self.assertEqual(status, 0, msg=f'{st} failed.\n{output}')


class CAMTestDomU2(CAMTest):

    @classmethod
    def setUpClass(cls):
        if not ('virtualization' in cls.td.get('IMAGE_FEATURES', '').split()):
            raise unittest.SkipTest("CAMTestDomU2 skipped because this build"
                                    " is not for virtualization architecture")

        if int(cls.td.get('DOMU_INSTANCES', 0)) < 2:
            raise unittest.SkipTest("CAMTestDomU2 skipped because "
                                    "DomU2 is not generated in this build")

        cls.si1_shell = Shell(
            cls.tc.target,
            ArmAutoSolutionsConfig.si_cl1_console_name, cls.tc.logger)

        cls.dom = DomU2(cls.tc)
        cls.lt_utils = cls.dom.get_lt_utils()

    @classmethod
    def tearDownClass(cls):
        cls.dom.shut_down()

    @OETestDepends(['test_10_linuxlogin.LinuxLoginTest.test_linux_login'])
    def test_cam_ptp_sync(self):
        super().test_cam_ptp_sync()

    @OETestDepends([
        'test_40_cam.CAMServiceTest.test_cam_service_boot_on_si',
        'test_40_cam.CAMTestDomU2.test_cam_tool_pack'])
    def test_cam_tool_deploy_to_si(self):
        super().test_cam_tool_deploy_to_si()

    @OETestDepends(['test_40_cam.CAMTestDomU2.test_cam_tool_deploy_to_si'])
    def test_cam_app_example_to_service_on_si(self):
        super().test_cam_app_example_to_service_on_si()

    @OETestDepends([
        'test_40_cam.CAMTestDomU2.test_cam_app_example_to_service_on_si'])
    def test_cam_app_example_to_service_on_si_with_multiple_connections(self):
        (
            super().
            test_cam_app_example_to_service_on_si_with_multiple_connections()
        )

    @OETestDepends([
        'test_40_cam.CAMTestDomU2.'
        'test_cam_app_example_to_service_on_si_with_multiple_connections'])
    def test_logical_check_on_si(self):
        super().test_logical_check_on_si()

    @OETestDepends(['test_40_cam.CAMTestDomU2.test_logical_check_on_si'])
    def test_temporal_check_on_si(self):
        super().test_temporal_check_on_si()

    @OETestDepends(['test_40_cam.CAMTestDomU2.test_cam_ptp_sync'])
    def test_data_calibration(self):
        super().test_data_calibration()

    @OETestDepends(['test_40_cam.CAMTestDomU2.test_data_calibration'])
    def test_cam_tool_pack(self):
        super().test_cam_tool_pack()


class CAMTestMultiDom(OERuntimeTestCase):
    @classmethod
    def setUpClass(cls):
        if not ('virtualization' in cls.td.get('IMAGE_FEATURES', '').split()):
            raise unittest.SkipTest("CAMTestDomU2 skipped because this build "
                                    "is not for virtualization architecture")

        if int(cls.td.get('DOMU_INSTANCES', 0)) < 2:
            raise unittest.SkipTest("CAMTestDomU2 skipped because "
                                    "DomU2 is not generated in this build")

        super(CAMTestMultiDom, cls).setUpClass()

        cls.domu1 = DomU1(cls.tc)
        cls.domu2 = DomU2(cls.tc)

    @classmethod
    def tearDownClass(cls):
        cls.domu1.shut_down()
        cls.domu2.shut_down()

        super(CAMTestMultiDom, cls).tearDownClass()

    @OETestDepends([
        'test_40_cam.CAMTest.test_temporal_check_on_si',
        'test_40_cam.CAMTestDomU2.test_temporal_check_on_si'])
    def test_cam_app_example_to_service_on_si_with_multiple_vms(self):
        multi_term = LinuxMultiTermUtils()
        for domu in (self.domu1, self.domu2):
            multi_term.add_cmd(
                (f'cam-app-example -u {domu.uuid_base}'
                 f' -a {ArmAutoSolutionsConfig.si_cl1_ipaddr}'
                 f' --processing-count {domu.processing_count}'
                 f' --stream-count {domu.streams}'),
                domu.get_lt_utils(),
                60*domu.streams
            )

        results = multi_term.run_concurrent()
        self.assertEqual(len(results), 2, msg='cam-app-example failed to run.')

        for status, _ in results:
            self.assertEqual(status, 0, msg='cam-app-example failed.')

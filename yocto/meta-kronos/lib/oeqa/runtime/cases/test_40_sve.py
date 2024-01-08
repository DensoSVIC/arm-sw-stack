#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

import re
from oeqa.runtime.case import OERuntimeTestCase
from oeqa.core.decorator.depends import OETestDepends
from oeqa.utils.xen_utils import XenUtils


class SVETest(OERuntimeTestCase):

    def _get_sve_from_xen_config(self, file_content):
        sve_match = re.search(r'sve\s*=\s*"(\d+)"', file_content)
        if sve_match:
            return int(sve_match.group(1))

    def _get_sve_default_length(self):
        status, sve_default_length_output = self.target.run(
                f'cat /proc/sys/abi/sve_default_vector_length', timeout=300)
        self.assertEqual(status, 0,
                         msg=f'Failed on read '
                         f'/proc/sys/abi/sve_default_vector_length')
        return int(sve_default_length_output.strip()) * 8

    def _test_sve_domu(self, hostname):
        # Set up variables for the specific guest domain
        console = self.tc.target._get_terminal(self.tc.target.DEFAULT_CONSOLE)
        dom0_prompt = \
            rf'root@(?!{hostname})fvp-rd-kronos:~#'
        linux_prompt = rf'root@{hostname}:~#'

        # Re-enter the guest domain using XenUtils
        XenUtils.enter_guest_from_dom0(console, dom0_prompt,
                                       linux_prompt, hostname)

        # Run in domu
        sve_default_length = self._get_sve_default_length()

        # Exit the guest domain using XenUtils
        XenUtils.exit_guest_to_dom0(console, dom0_prompt,
                                    linux_prompt, hostname)

        # Read domu.cfg content from dom0
        status, xen_config_content = self.target.run(
                    f'cat /etc/xen/auto/{hostname}.cfg', timeout=300)
        self.assertEqual(status, 0,
                         msg=f'Failed on read /etc/xen/auto/{hostname}.cfg')

        # Extract SVE from domu.cfg in dom0
        sve_from_config = self._get_sve_from_xen_config(xen_config_content)

        self.assertEqual(sve_from_config, sve_default_length,
                         msg="Vector lengths do not match.")

    def _run_xl_info(self):
        status, xl_info_output = self.target.run('xl info', timeout=300)
        self.assertEqual(status, 0,
                         msg=f'Failed on run xl info')
        return xl_info_output

    def _get_arm_sve_vector_length(self, xl_info_output):
        for line in xl_info_output.split('\n'):
            if "arm_sve_vector_length" in line:
                return int(line.split(":")[1].strip())

    @OETestDepends(['test_10_linuxlogin.LinuxLoginTest.test_linux_login'])
    def test_sve_enabled(self):
        # Run in host
        status, cpuinfo_output = self.target.run('cat /proc/cpuinfo',
                                                 timeout=300)
        self.assertEqual(status, 0,
                         msg='Failed on read /proc/cpuinfo')
        self.assertTrue(" sve2 " in cpuinfo_output.lower(),
                        msg="SVE2 not enabled")

    @OETestDepends(['test_10_linuxlogin.LinuxLoginTest.test_linux_login'])
    def test_sve_config(self):
        """
        Test the Scalable Vector Extension (SVE) configuration in
        virtualization and baremetal.

        In virtualization:
        This test checks the SVE vector length configuration in domu1 and domu2
        against their respective configuration files (/etc/xen/auto/domuX.cfg
        for DomUs) and the command line for Dom0.

        In baremetal, it assumes a default SVE vector length of 128.
        """
        image_features = self.td.get('IMAGE_FEATURES')
        if 'virtualization' in image_features.split():
            # Run tests in domu1 and domu2
            self._test_sve_domu('domu1')
            self._test_sve_domu('domu2')

            # Check arm_sve_vector_length using xl info in dom0
            xl_info = self._run_xl_info()
            arm_sve_vector_length = self._get_arm_sve_vector_length(xl_info)
        else:
            arm_sve_vector_length = 128

        sve_default_length = self._get_sve_default_length()

        self.assertEqual(arm_sve_vector_length, sve_default_length,
                         msg='Vector lengths do not match')

#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

import re
import unittest
from oeqa.core.decorator.depends import OETestDepends
from oeqa.runtime.case import OERuntimeTestCase
from oeqa.utils.linux_terminal_utils import LinuxTermUtils
from oeqa.utils.arm_auto_solutions_config import ArmAutoSolutionsConfig
from oeqa.utils.xen_utils import XenUtils


class SVETestBase(OERuntimeTestCase):
    @classmethod
    def setUpClass(cls):
        super(SVETestBase, cls).setUpClass()
        cls.prompt = ArmAutoSolutionsConfig.baremetal_prompt
        cls.linux_console = cls.tc.target._get_terminal("default")
        cls.lt_utils = LinuxTermUtils(cls.tc, cls.linux_console, cls.prompt)

    def _get_sve_vector_length(self):
        status, sve_default_length_output = self.lt_utils.run(
            "cat /proc/sys/abi/sve_default_vector_length", timeout=300
        )
        self.assertEqual(
            status, 0, msg="Failed on read "
                           "/proc/sys/abi/sve_default_vector_length"
        )
        return int(sve_default_length_output.strip()) * 8

    def _get_sve_vl_from_commandline(self):
        status, xl_info_output = self.lt_utils.run("xl info", timeout=300)
        self.assertEqual(status, 0, msg="Failed on run xl info")

        xen_commandline_line = next(
            (
                line
                for line in xl_info_output.split("\n")
                if line.startswith("xen_commandline")
            ),
            None,
        )

        if not xen_commandline_line:
            return

        prefix = "dom0=sve="
        for part in xen_commandline_line.split():
            if part.startswith(prefix):
                return int(part[len(prefix):])

    def _get_configured_sve_vl(self):
        if "virtualization" in self.td.get("IMAGE_FEATURES").split():
            return self._get_sve_vl_from_commandline()

        # Currently FVP offers 128 SVE vector length
        return 128

    @OETestDepends(["test_10_linuxlogin.LinuxLoginTest.test_linux_login"])
    def test_sve_enabled(self):
        status, cpuinfo_output = self.lt_utils.run("cat /proc/cpuinfo",
                                                   timeout=300)
        self.assertEqual(status, 0, msg="Failed on read /proc/cpuinfo")
        self.assertIn("sve2", cpuinfo_output.lower(), msg="SVE2 not enabled")

    @OETestDepends(['test_40_sve.SVETestBase.test_sve_enabled'])
    def test_sve_config(self):
        configured_sve_vector_length = self._get_configured_sve_vl()
        sve_vector_length = self._get_sve_vector_length()

        self.assertEqual(
            configured_sve_vector_length,
            sve_vector_length,
            msg="SVE vector length do not match",
        )


class SVETestDomU1(SVETestBase):
    domu_hostname = ArmAutoSolutionsConfig.domu1_hostname

    @classmethod
    def setUpClass(cls):
        if "virtualization" not in cls.td.get("IMAGE_FEATURES", "").split():
            raise unittest.SkipTest(
                f"{cls.__name__} skipped because"
                " 'virtualization' is not in IMAGE_FEATURES"
            )
        super(SVETestDomU1, cls).setUpClass()

        cls.lt_utils_dom0 = LinuxTermUtils(cls.tc, cls.linux_console,
                                           cls.prompt)
        cls.linux_prompt = "root@{}:~#".format(cls.domu_hostname)
        cls.domu_console = LinuxTermUtils.open_ssh_shell(
            cls.tc.target, cls.domu_hostname, cls.tc.logger
        )
        XenUtils.enter_guest_from_dom0(
            cls.domu_console, cls.prompt, cls.linux_prompt, cls.domu_hostname
        )
        cls.lt_utils = LinuxTermUtils(cls.tc, cls.domu_console,
                                      cls.linux_prompt)

    @classmethod
    def tearDownClass(cls):
        XenUtils.exit_guest_to_dom0(
            cls.lt_utils.console,
            cls.prompt,
            cls.linux_prompt,
            cls.domu_hostname,
            False,
        )
        LinuxTermUtils.close_ssh_shell(cls.lt_utils.console,
                                       cls.tc.logger)
        super(SVETestDomU1, cls).tearDownClass()

    def _get_sve_vl_from_xl_config(self):
        file_content = self._read_xen_config()
        sve_match = re.search(r'sve\s*=\s*"(\d+|disabled|hw)"', file_content)

        if sve_match:
            sve_value = sve_match.group(1)
            if sve_value == "hw":
                return self._get_max_sve_vector_length()
            elif sve_value == "disabled":
                return 0
            else:
                return int(sve_value)

    def _read_xen_config(self):
        status, file_content = self.lt_utils_dom0.run(
            f"cat /etc/xen/auto/{self.domu_hostname}.cfg", timeout=300
        )
        self.assertEqual(
            status, 0, msg="Failed on read "
            f"/etc/xen/auto/{self.domu_hostname}.cfg"
        )
        return file_content

    def _get_max_sve_vector_length(self):
        status, xl_info_output = self.lt_utils_dom0.run("xl info", timeout=300)
        for line in xl_info_output.split("\n"):
            if "arm_sve_vector_length" in line:
                return int(line.split(":")[1].strip())

    def _get_configured_sve_vl(self):
        # Retrieve SVE vector length from the xl configuration file
        return self._get_sve_vl_from_xl_config()


class SVETestDomU2(SVETestDomU1):
    domu_hostname = ArmAutoSolutionsConfig.domu2_hostname

    @classmethod
    def setUpClass(cls):
        if int(cls.td.get('DOMU_INSTANCES', 0)) < 2:
            raise unittest.SkipTest("SVETestDomU2 skipped because DomU2 is"
                                    " not generated in this build")
        super().setUpClass()

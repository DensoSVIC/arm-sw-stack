#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.runtime.cases.fvp_devices import FvpDevicesTest
from oeqa.utils.arm_auto_solutions_config import ArmAutoSolutionsConfig
from oeqa.utils.linux_terminal_utils import LinuxTermUtils


class ArmAutoSolutionsFvpDevicesTest(FvpDevicesTest):

    @classmethod
    def setUpClass(cls):
        super(ArmAutoSolutionsFvpDevicesTest, cls).setUpClass()
        cls.prompt = ArmAutoSolutionsConfig.baremetal_prompt
        cls.linux_console = cls.tc.target._get_terminal("default")
        cls.lt_utils = LinuxTermUtils(cls.tc, cls.linux_console, cls.prompt)

    def run_cmd(self, cmd, check=True):
        status, output = self.lt_utils.run(cmd, timeout=300)
        if status and check:
            self.fail("Command '%s' returned non-zero exit "
                      "status %d:\n%s" % (cmd, status, output))

        return (status, output)

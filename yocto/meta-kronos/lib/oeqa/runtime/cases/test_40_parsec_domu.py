#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.runtime.cases.parsec import ParsecTest
from oeqa.utils.xen_utils import XenUtils


class ParsecDomU1Test(ParsecTest):
    domu_hostname = r'domu1'

    @classmethod
    def setUpClass(cls):
        super(ParsecDomU1Test, cls).setUpClass()
        cls.linux_console = cls.tc.target.DEFAULT_CONSOLE
        # Use negative lookahead to match Dom0 prompt, so match every prompt
        # that is not of this guest
        cls.dom0_prompt = \
            rf'root@(?!{cls.domu_hostname})fvp-rd-kronos:~#'
        cls.linux_prompt = rf'root@{cls.domu_hostname}:~#'
        cls.console = cls.tc.target._get_terminal(cls.linux_console)
        XenUtils.enter_guest_from_dom0(cls.console, cls.dom0_prompt,
                                       cls.linux_prompt, cls.domu_hostname)

    @classmethod
    def tearDownClass(cls):
        XenUtils.exit_guest_to_dom0(cls.console, cls.dom0_prompt,
                                    cls.linux_prompt, cls.domu_hostname)
        super(ParsecDomU1Test, cls).tearDownClass()


class ParsecDomU2Test(ParsecDomU1Test):
    domu_hostname = r'domu2'

    @classmethod
    def setUpClass(cls):
        if int(cls.td.get('DOMU_INSTANCES', 0)) < 2:
            import unittest
            raise unittest.SkipTest("ParsecDomU2Test skipped because DomU2 is"
                                    " not generated in this build")
        super(ParsecDomU2Test, cls).setUpClass()

    @classmethod
    def tearDownClass(cls):
        super(ParsecDomU2Test, cls).tearDownClass()

#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.core.decorator.depends import OETestDepends
from oeqa.runtime.cases.test_30_hipc import HIPCTestBase
from oeqa.utils.xen_utils import XenUtils


class HIPCTestDomU1(HIPCTestBase):
    domu_hostname = r'domu1'

    @classmethod
    def setUpClass(cls):
        super(HIPCTestDomU1, cls).setUpClass()
        # Use negative lookahead to match Dom0 prompt, so match every prompt
        # that is not of this guest
        cls.dom0_prompt = rf'root@(?!{cls.domu_hostname}){cls.hostname}:~#'
        cls.linux_prompt = rf'root@{cls.domu_hostname}:~#'
        cls.console = \
            cls.tc.target._get_terminal(cls.tc.target.DEFAULT_CONSOLE)
        cl0_shell = cls.tc.target._get_terminal('safety_island_c0')
        cl1_shell = cls.tc.target._get_terminal('safety_island_c1')
        cl2_shell = cls.tc.target._get_terminal('safety_island_c2')
        cls.console.delaybeforesend = 0.2
        cl0_shell.delaybeforesend = 0.2
        cl1_shell.delaybeforesend = 0.2
        cl2_shell.delaybeforesend = 0.2
        XenUtils.enter_guest_from_dom0(cls.console, cls.dom0_prompt,
                                       cls.linux_prompt, cls.domu_hostname)

    @classmethod
    def tearDownClass(cls):
        XenUtils.exit_guest_to_dom0(cls.console, cls.dom0_prompt,
                                    cls.linux_prompt, cls.domu_hostname)
        super(HIPCTestDomU1, cls).tearDownClass()

    @OETestDepends(
            ['test_30_hipc_virtualization.HIPCTestDomU1.test_ping_cluster0'])
    def test_hipc_cluster0(self):
        super().test_hipc_cluster0()

    @OETestDepends(
            ['test_30_hipc_virtualization.HIPCTestDomU1.test_ping_cluster1'])
    def test_hipc_cluster1(self):
        super().test_hipc_cluster1()

    @OETestDepends(
            ['test_30_hipc_virtualization.HIPCTestDomU1.test_ping_cluster2'])
    def test_hipc_cluster2(self):
        super().test_hipc_cluster2()

    def test_ping_cl0_cl1(self):
        super().test_ping_cl0_cl1()

    def test_ping_cl0_cl2(self):
        super().test_ping_cl0_cl2()

    def test_ping_cl1_cl2(self):
        super().test_ping_cl1_cl2()

    @OETestDepends(
            ['test_30_hipc_virtualization.HIPCTestDomU1.test_ping_cl0_cl1'])
    def test_hipc_cluster_cl0_cl1(self):
        super().test_hipc_cluster_cl0_cl1()

    @OETestDepends(
            ['test_30_hipc_virtualization.HIPCTestDomU1.test_ping_cl0_cl2'])
    def test_hipc_cluster_cl0_cl2(self):
        super().test_hipc_cluster_cl0_cl2()

    @OETestDepends(
            ['test_30_hipc_virtualization.HIPCTestDomU1.test_ping_cl1_cl2'])
    def test_hipc_cluster_cl1_cl2(self):
        super().test_hipc_cluster_cl1_cl2()


class HIPCTestDomU2(HIPCTestDomU1):
    domu_hostname = r'domu2'

    @classmethod
    def setUpClass(cls):
        if int(cls.td.get('DOMU_INSTANCES', 0)) < 2:
            import unittest
            raise unittest.SkipTest("HIPCTestDomU2 skipped because DomU2 is"
                                    " not generated in this build")
        super(HIPCTestDomU2, cls).setUpClass()

    @classmethod
    def tearDownClass(cls):
        super(HIPCTestDomU2, cls).tearDownClass()

    def test_ping_cluster0(self):
        self.skipTest("Ping to Cluster 0 not tested for DomU2")

    @OETestDepends(['test_10_linuxlogin.LinuxLoginTest.test_linux_login'])
    def test_ping_cluster1(self):
        self.ping(r'192.168.1.1', 'safety_island_c1', r'192.168.1.3', 200)

    def test_ping_cluster2(self):
        self.skipTest("Ping to Cluster 2 not tested for DomU2")

    def test_hipc_cluster0(self):
        self.skipTest("HIPC to Cluster 0 not tested for DomU2")

    @OETestDepends(
            ['test_30_hipc_virtualization.HIPCTestDomU2.test_ping_cluster1'])
    def test_hipc_cluster1(self):
        self.hipc(r'192.168.1.1', 'safety_island_c1', r'192.168.1.3')

    def test_hipc_cluster2(self):
        self.skipTest("HIPC to Cluster 2 not tested for DomU2")

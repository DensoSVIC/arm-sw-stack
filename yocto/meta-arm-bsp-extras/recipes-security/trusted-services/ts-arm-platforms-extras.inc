#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SRCREV_trusted-services:fvp-rd-kronos = "08b3d39471f4914186bd23793dc920e83b0e3197"
SRCREV_mbedtls:fvp-rd-kronos = "8c89224991adff88d53cd380f42a2baa36f91454"
SRCREV_nanopb:fvp-rd-kronos = "df0e92f474f9cca704fe2b31483f0b4d1b1715a4"

LIC_FILES_CHKSUM:remove:fvp-rd-kronos = "file://../mbedtls/LICENSE;md5=379d5819937a6c2f1ef1630d341e026d"
LIC_FILES_CHKSUM:append:fvp-rd-kronos = " file://../mbedtls/LICENSE;md5=3b83ef96387f14655fc854ddc3c6bd57"

FILESEXTRAPATHS:prepend:fvp-rd-kronos := "${THISDIR}/files:"
COMPATIBLE_MACHINE:fvp-rd-kronos = "fvp-rd-kronos"
SRC_URI:append:fvp-rd-kronos = " \
    file://0001-Add-new-framework-for-kronos-platform.patch \
    file://0002-Add-mhu-v3-driver-for-se_proxy-SP.patch \
    file://0003-Add-new-rpc-caller-for-Protected-Storage-in-se_proxy.patch \
    file://0004-Increase-MHU-buffer-and-payload-size-for-rss_comms.patch \
    file://0005-Change-the-base-address-for-rss_comms-virtio.patch \
    file://0006-Add-new-rpc-caller-for-Crypto-Service-on-kronos-plat.patch \
    file://0007-Plat-fvp-rd-kronos-Change-default-smm-values.patch \
    file://0008-Modify-the-device-name-from-xx-virtio-to-xx-shmem.patch \
    file://0009-Add-stub-capsule-update-service-components.patch \
    file://0010-Fixes-in-AEAD-for-psa-arch-test-54-and-58.patch \
    file://0011-FMP-Support-in-Corstone1000.patch \
    file://0012-Plat-corstone1000-add-compile-definitions-for-ECP_DP.patch \
    file://0013-Plat-corstone1000-Use-the-stateless-platform-service.patch \
    file://0014-Plat-corstone1000-Initialize-capsule-update-provider.patch \
    file://0015-plat-Rename-symbol-names-to-common-ones.patch \
    file://0016-Plat-kronos-Enable-capsule-update-service.patch \
    file://0017-tools-b-test-Add-se-proxy-kronos-opteesp-to-b-test.patch \
    file://0018-Plat-kronos-Add-ns-interrupts-action-for-OP-TEE-3.22.patch \
    file://0019-smm_gateway-GetNextVariableName-Fix.patch \
    file://0020-LazyFetch-allow-setting-the-cmake-generator.patch \
    file://0021-Plat-kronos-Add-internal-trusted-storage-SE-Proxy-de.patch \
    "
EXTRA_OECMAKE:append:fvp-rd-kronos = " -D PLAT_MHU_VERSION=3"

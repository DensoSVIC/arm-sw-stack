#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend:fvp-rd-kronos := "${THISDIR}/files:"
COMPATIBLE_MACHINE:fvp-rd-kronos = "fvp-rd-kronos"
SRC_URI:append:fvp-rd-kronos = " \
    file://0001-Add-new-framework-for-kronos-platform.patch \
    file://0002-Add-mhu-v3-driver-for-se_proxy-SP.patch \
    file://0003-Introduce-MHUv3-adapter.patch \
    file://0004-Introduce-outband-message-buffer.patch \
    file://0005-Kronos-Change-the-encoding-of-RSS-message.patch \
    file://0006-Enable-rss_comm-based-RPC-for-se-proxy-on-Kronos.patch \
    file://0007-Add-new-rpc-caller-for-Crypto-Service-on-kronos-plat.patch \
    file://0008-Plat-fvp-rd-kronos-Change-default-smm-values.patch \
    file://0009-Add-stub-capsule-update-service-components.patch \
    file://0010-Fixes-in-AEAD-for-psa-arch-test-54-and-58.patch \
    file://0011-FMP-Support-in-Corstone1000.patch \
    file://0012-smm_gateway-GetNextVariableName-Fix.patch \
    file://0013-Plat-corstone1000-Add-compile-definitions-for-ECP_DP.patch \
    file://0014-Plat-corstone1000-Call-psa_call-with-existing-handle.patch \
    file://0015-Plat-corstone1000-Initialize-capsule-update-provider.patch \
    file://0016-Plat-Rename-symbol-names-to-common-ones.patch \
    file://0017-Plat-kronos-Enable-capsule-update-service.patch \
    file://0018-tools-b-test-Add-se-proxy-kronos-opteesp-to-b-test.patch \
    file://0019-Allow-some-warnings-in-compilation.patch \
    file://0020-Remove-PLATFORM_HAS_ATTEST_PK-define-from-IAT-test.patch \
    file://0021-Fix-Avoid-redefinition-of-variables.patch \
    file://0022-Fix-GetNextVariableName-NameSize-input.patch \
    file://0023-Fix-error-handling-of-variable-index-loading.patch \
    file://0024-Provide-crypto-api-to-create-UEFI-var-finger-print.patch \
    file://0025-Add-timestamp-validation-for-uefi-variables.patch \
    file://0026-Isolate-common-uefi-variable-authentication-steps.patch \
    file://0027-Verify-Private-Authenticated-Variables.patch \
    file://0028-Enable-Crypto-Protocol-Buffers-in-SE-Proxy.patch \
    file://0029-Enlarge-the-payload-size-of-RSS-message.patch \
    file://0030-Plat-kronos-Add-internal-trusted-storage-SE-Proxy-de.patch \
    "

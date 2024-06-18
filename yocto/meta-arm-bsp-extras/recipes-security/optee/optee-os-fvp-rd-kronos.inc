#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend := "${THISDIR}/files/optee-os/fvp-rd-kronos:"

# cSpell:disable
SRC_URI:append = " \
                  file://0001-core-arm-add-MPIDR-affinity-shift-and-mask-for-32-bi.patch \
                  file://0002-Handle-logging-syscall.patch \
                  file://0003-core-arm-add-rdkronos-platform.patch \
                  file://0004-core-arm-Reduce-the-OPTEE-ram-size-to-4M-for-rdkrono.patch \
                  file://0005-core-arm-Remove-hardcoded-log-level-value.patch \
                  file://0006-ta-pkcs11-Improve-PIN-counter-handling-robustness.patch \
                 "
# cSpell:enable

COMPATIBLE_MACHINE = "fvp-rd-kronos"

OPTEEMACHINE = "rdkronos"

EXTRA_OEMAKE += " CFG_CORE_FFA=y \
                  CFG_WITH_SP=y \
                  CFG_DT=y \
                  CFG_TEE_BENCHMARK=n \
                  CFG_TEE_CORE_LOG_LEVEL=3 \
                  CFG_SECURE_PARTITION=y \
                  CFG_MAP_EXT_DT_SECURE=y \
                  "

TS_INSTALL_PREFIX_PATH = "${RECIPE_SYSROOT}/firmware/sp/opteesp"

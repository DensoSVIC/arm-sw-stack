#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: Apache-2.0

if (CONFIG_TFM_PSA_PROTECTED_STORAGE_API OR CONFIG_TFM_PSA_INTERNAL_TRUSTED_STORAGE_API)
zephyr_library_named(tfm)

zephyr_include_directories(
        ${ZEPHYR_TRUSTED_FIRMWARE_M_MODULE_DIR}/interface/include
)

zephyr_library_sources_ifdef(
        CONFIG_TFM_PSA_PROTECTED_STORAGE_API
        ${ZEPHYR_TRUSTED_FIRMWARE_M_MODULE_DIR}/interface/src/tfm_ps_api.c
)

zephyr_library_sources_ifdef(
        CONFIG_TFM_PSA_INTERNAL_TRUSTED_STORAGE_API
        ${ZEPHYR_TRUSTED_FIRMWARE_M_MODULE_DIR}/interface/src/tfm_its_api.c
)
endif()

#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: Apache-2.0

if (CONFIG_PSA_ARCH_TEST_PROTECTED_STORAGE)
        set(PSA_ARCH_TEST_SUITE PROTECTED_STORAGE)
elseif (CONFIG_PSA_ARCH_TEST_INTERNAL_TRUSTED_STORAGE)
        set(PSA_ARCH_TEST_SUITE INTERNAL_TRUSTED_STORAGE)
elseif (CONFIG_PSA_ARCH_TEST_STORAGE)
        set(PSA_ARCH_TEST_SUITE STORAGE)
elseif (CONFIG_PSA_ARCH_TEST_CRYPTO)
        set(PSA_ARCH_TEST_SUITE CRYPTO)
endif()

if(PSA_ARCH_TEST_SUITE)

set(PSA_ARCH_TESTS_PATH ${ZEPHYR_CURRENT_MODULE_DIR}/../psa-arch-tests)

zephyr_library_sources(src/zephyr_psa_test.c)

if (CONFIG_PSA_ARCH_TEST_API_TFM_HEADERS)
        set(PSA_INCLUDE_PATHS ${ZEPHYR_TRUSTED_FIRMWARE_M_MODULE_DIR}/interface/include)
elseif (CONFIG_PSA_ARCH_TEST_API_MBEDTLS_HEADERS)
        set(PSA_INCLUDE_PATHS ${ZEPHYR_MBEDTLS_MODULE_DIR}/include)
endif()

set(SUITE ${PSA_ARCH_TEST_SUITE})
set(TOOLCHAIN INHERIT)
set(TARGET tgt_dev_apis_linux)
add_subdirectory(${PSA_ARCH_TESTS_PATH}/api-tests psa-arch-tests-zephyr)
zephyr_link_libraries(val_nspe pal_nspe test_combine zephyr_interface)

endif()

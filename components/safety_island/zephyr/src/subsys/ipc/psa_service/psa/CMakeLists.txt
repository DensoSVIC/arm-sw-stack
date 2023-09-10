#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: Apache-2.0

zephyr_library()

zephyr_library_sources_ifdef(CONFIG_PSA_INTERNAL_TRUSTED_STORAGE internal_trusted_storage.c)
zephyr_library_sources_ifdef(CONFIG_PSA_PROTECTED_STORAGE protected_storage.c)

#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: Apache-2.0

zephyr_library()

zephyr_library_sources(fault_mgmt.c)
zephyr_library_sources_ifdef(CONFIG_FAULT_MGMT_SAFETY fault_mgmt_safety.c)
zephyr_library_sources_ifdef(CONFIG_FAULT_MGMT_SHELL fault_mgmt_shell.c)
zephyr_library_sources_ifdef(CONFIG_FAULT_MGMT_STORAGE_SYS_HASH_MAP fault_mgmt_storage.c fault_mgmt_storage_sys_hash_map.c)
zephyr_library_sources_ifdef(CONFIG_FAULT_MGMT_PSA_PROTECTED_STORAGE fault_mgmt_storage.c fault_mgmt_psa_protected_storage.c)

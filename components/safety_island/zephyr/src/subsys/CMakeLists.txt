#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: Apache-2.0

add_subdirectory_ifdef(CONFIG_FAULT_MGMT fault_mgmt)
add_subdirectory_ifdef(CONFIG_PSA ipc/psa_service)
add_subdirectory_ifdef(CONFIG_PSA ipc/psa_service/rss_comms)
add_subdirectory_ifdef(CONFIG_PSA ipc/psa_service/psa)

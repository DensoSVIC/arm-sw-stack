#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: Apache-2.0

zephyr_library()

zephyr_library_sources(
        psa_backend.c
        psa_service.c
)

add_subdirectory(rss_comms)

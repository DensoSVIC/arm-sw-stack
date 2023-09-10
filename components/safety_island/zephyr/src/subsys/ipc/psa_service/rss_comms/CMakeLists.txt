#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: Apache-2.0

zephyr_library()

zephyr_library_sources_ifdef(CONFIG_PSA rss_comms_protocol_embed.c)
zephyr_library_sources_ifdef(CONFIG_PSA rss_comms_protocol_pointer_access.c)
zephyr_library_sources_ifdef(CONFIG_PSA rss_comms_protocol.c)

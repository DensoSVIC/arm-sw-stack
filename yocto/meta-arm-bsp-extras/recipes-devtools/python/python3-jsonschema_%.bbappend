#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# python3-jsonschema_4.21.1 is incompatible with python3-dtschema_2024.2
# jsonschema [required: >=4.1.2,<4.18]
# need python3-jsonschema_4.17.3
# oelint-adv does not support interpolated include paths
# nooelint: oelint.file.includenotfound
include python3-jsonschema-${MACHINE}.inc

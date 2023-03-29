#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

require ${@bb.utils.contains('EXTRA_IMAGE_FEATURES', 'virtualization', "${KRONOS_VIRTUALIZATION_DYNAMIC_DIR}/conf/machine/include/fvp-rd-kronos-xen.inc", '', d)}

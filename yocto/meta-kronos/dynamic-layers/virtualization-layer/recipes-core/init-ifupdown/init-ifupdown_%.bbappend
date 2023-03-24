#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS:prepend:fvp-rd-kronos := \
                        "${@bb.utils.contains('EXTRA_IMAGE_FEATURES', \
                         'virtualization', \
                         '${THISDIR}/files:', \
                         '', d)}"

# Dom0 xen bridge network configuration
SRC_URI:append:fvp-rd-kronos = \
    " file://interfaces.d/1001-xen-bridge"

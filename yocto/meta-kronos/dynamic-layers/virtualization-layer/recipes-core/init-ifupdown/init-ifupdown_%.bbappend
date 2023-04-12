#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

OVERRIDES:append = "${@bb.utils.contains('DISTRO_FEATURES', \
                    'xen', \
                    ':xen', \
                    '', d)}"

FILESEXTRAPATHS:prepend:fvp-rd-kronos:xen := \
                        "${@bb.utils.contains('EXTRA_IMAGE_FEATURES', \
                         'virtualization', \
                         '${THISDIR}/files:', \
                         '', d)}"

# Dom0 xen bridge network configuration
SRC_URI:append:fvp-rd-kronos:xen = \
    " file://interfaces.d/1001-xen-bridge"

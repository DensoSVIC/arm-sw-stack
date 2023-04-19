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

# DomU Safety Island virtual network configuration fixup for eth name
do_install:append:generic-arm64 () {
    cat <<EOF >> ${WORKDIR}/interfaces.d/0002-safety-island-c0
    pre-up ip link set eth1 name ethsi0
    post-down ip link set ethsi0 name eth1
EOF

cat <<EOF >> ${WORKDIR}/interfaces.d/0002-safety-island-c1
    pre-up ip link set eth2 name ethsi1
    post-down ip link set ethsi1 name eth2
EOF

cat <<EOF >> ${WORKDIR}/interfaces.d/0002-safety-island-c2
    pre-up ip link set eth3 name ethsi2
    post-down ip link set ethsi2 name eth3
EOF
}

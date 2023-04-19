#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

OVERLAY_BASENAME ?= "${ZEPHYR_SAFETY_ISLAND_MODULE}/overlays/hipc/${ZEPHYR_BOARD}"

python __anonymous() {
    ovrl_basename = d.getVar('OVERLAY_BASENAME')
    if ovrl_basename:
        extra_conf = f' -DDTC_OVERLAY_FILE={ovrl_basename}.overlay' \
                     f' -DOVERLAY_CONFIG={ovrl_basename}.conf'
        d.appendVar('EXTRA_OECMAKE', extra_conf)
}

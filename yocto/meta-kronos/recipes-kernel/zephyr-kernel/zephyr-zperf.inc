#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

OVERLAY_DIR ?= "hipc"

OVERLAY_BASENAME ?= \
    "${ZEPHYR_SAFETY_ISLAND_MODULE}/overlays/${OVERLAY_DIR}/${ZEPHYR_BOARD}"
OVERLAY_BRIDGE_BASENAME ?= \
    "${ZEPHYR_SAFETY_ISLAND_MODULE}/overlays/bridge/${ZEPHYR_BOARD}"

python __anonymous() {
    ovrl_basename = d.getVar('OVERLAY_BASENAME')
    ovrl_bridge_basename = d.getVar('OVERLAY_BRIDGE_BASENAME')
    extra_image_features = d.getVar('EXTRA_IMAGE_FEATURES', "")

    if ovrl_basename and ovrl_bridge_basename:
        if 'si0-bridge-ethernet0' in extra_image_features:
            extra_conf = f' -DDTC_OVERLAY_FILE={ovrl_basename}.overlay' \
                         f' -DOVERLAY_CONFIG="{ovrl_basename}.conf;{ovrl_bridge_basename}.conf"'
        else:
            extra_conf = f' -DDTC_OVERLAY_FILE={ovrl_basename}.overlay' \
                         f' -DOVERLAY_CONFIG={ovrl_basename}.conf'

        d.appendVar('EXTRA_OECMAKE', extra_conf)
}

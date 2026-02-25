SUMMARY = "EBS Safety Island"
DESCRIPTION = "EBS Safety Island"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://license.rst;md5=214c73aa30e7f71d6173261fe950f51b"

require recipes-kernel/zephyr-kernel/zephyr-sample.inc

ZEPHYR_SRC_DIR = "${ZEPHYR_SAFETY_ISLAND_MODULE}/apps/ebs"

do_compile[network] = "1"

OVERLAY_BASENAME_FILE = "${ZEPHYR_SAFETY_ISLAND_MODULE}/overlays/hipc/${BOARD}"
OVERLAY_EBS_BASENAME_FILE = "${ZEPHYR_SAFETY_ISLAND_MODULE}/apps/ebs/boards/${BOARD}_ebs"
EXTRA_OECMAKE:append = "\
    -DDTC_OVERLAY_FILE='${OVERLAY_BASENAME_FILE}.overlay' \
    -DOVERLAY_CONFIG='${OVERLAY_BASENAME_FILE}.conf;${OVERLAY_EBS_BASENAME_FILE}.conf' \
    -DZEPHYR_MODULES='${ZEPHYR_MODULES};${ZEPHYR_SRC_DIR}/modules/libmicroxrcedds' \
"

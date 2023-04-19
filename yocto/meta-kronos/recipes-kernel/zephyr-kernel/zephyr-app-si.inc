#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

ZEPHYR_APP ??= "helloworld"

ZEPHYR_SRC_DIR ??= "${ZEPHYR_BASE}/samples/${ZEPHYR_APP}"
# If the extra inc file zephyr-xxx.inc exists, include it
ZEPHYR_INC_FILE = "zephyr-${ZEPHYR_APP}.inc"

# This 'include' directive causes BitBake to parse whatever file you
# specify, and to insert that file at that location. It does not produce
# an error when the file cannot be found.
include ${ZEPHYR_INC_FILE}

require recipes-kernel/zephyr-kernel/zephyr-${ZEPHYR_APP}.bb

PACKAGES += "${PN}-${ZEPHYR_APP}"

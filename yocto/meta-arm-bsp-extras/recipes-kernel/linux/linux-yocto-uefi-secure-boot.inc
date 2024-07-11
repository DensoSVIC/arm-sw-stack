#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

inherit uefi-secure-boot-image-signing

KERNEL_IMAGETYPE ?= "Image"

sign_kernel_image() {
    sign_image "${B}/${KERNEL_OUTPUT_DIR}/${KERNEL_IMAGETYPE}"
}

sign_kernel_image[doc] = "Sign the kernel image."

do_install[postfuncs] += "sign_kernel_image"

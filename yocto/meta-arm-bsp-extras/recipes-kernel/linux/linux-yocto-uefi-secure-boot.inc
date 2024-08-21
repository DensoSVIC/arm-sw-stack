#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

inherit uefi-secure-boot-image-signing

KERNEL_IMAGETYPE ?= "Image"
UNSIGNED_KERNEL_IMAGE ?= "${KERNEL_IMAGETYPE}.unsigned"

sign_kernel_image() {
    # Copy the kernel image before the signing operation.
    install -m 0644 "${B}/${KERNEL_OUTPUT_DIR}/${KERNEL_IMAGETYPE}" \
			"${B}/${KERNEL_OUTPUT_DIR}/${UNSIGNED_KERNEL_IMAGE}"

    sign_image "${B}/${KERNEL_OUTPUT_DIR}/${KERNEL_IMAGETYPE}"
}

sign_kernel_image[doc] = "Sign the kernel image."

do_install[postfuncs] += "sign_kernel_image"

do_deploy:append() {
    install -m 0644 "${B}/${KERNEL_OUTPUT_DIR}/${UNSIGNED_KERNEL_IMAGE}" \
			"${DEPLOYDIR}/${UNSIGNED_KERNEL_IMAGE}"
}

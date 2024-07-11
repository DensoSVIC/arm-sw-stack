#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

inherit uefi-secure-boot-image-signing

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append:fvp-rd-kronos = " \
        file://0001-verifiers-Don-t-return-error-for-deferred-image.patch \
        "

do_mkimage() {
	cd ${B}

	GRUB_MKIMAGE_MODULES="${GRUB_BUILDIN}"

	# If 'all' is included in GRUB_BUILDIN we will include all available grub2 modules
	if [ "${@ bb.utils.contains('GRUB_BUILDIN', 'all', 'True', 'False', d)}" = "True" ]; then
		bbdebug 1 "Including all available modules"
		# Get the list of all .mod files in grub-core build directory
		GRUB_MKIMAGE_MODULES=$(find ${B}/grub-core/ -type f -name "*.mod" -exec basename {} .mod \;)
	fi

	# Search for the grub.cfg on the local boot media by using the
	# built in cfg file provided via this recipe
	grub-mkimage --disable-shim-lock -v -c ../cfg -p ${EFIDIR} -d ./grub-core/ \
	               -O ${GRUB_TARGET}-efi -o ./${GRUB_IMAGE_PREFIX}${GRUB_IMAGE} \
	               ${GRUB_MKIMAGE_MODULES}
}

sign_grub_image() {
    sign_image "${B}/${GRUB_IMAGE_PREFIX}${GRUB_IMAGE}"
}

do_install[postfuncs] += "sign_grub_image"

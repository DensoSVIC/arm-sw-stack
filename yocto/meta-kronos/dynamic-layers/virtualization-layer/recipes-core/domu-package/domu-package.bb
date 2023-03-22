#
# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

SUMMARY = "Xen DomU package"
DESCRIPTION = "A recipe to bundle a DomU image and configuration as a package\
which can be installed in Dom0"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "\
    file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302 \
    "

include domu-envs.inc

inherit features_check
REQUIRED_IMAGE_FEATURES += "virtualization"

DEPENDS += "gettext-native"

do_patch[noexec] = "1"
do_configure[noexec] = "1"
do_compile[noexec] = "1"

DOMU_KERNEL_IMG = "Image-${DOMU_MACHINE}.bin"
DOMU_DIR = "${datadir}/domus"
XEN_AUTO_DIR = "${sysconfdir}/xen/auto"

SRC_URI = "file://domu-conf.sample"


# Make a copy of kernel image and rootfs for each domus
DOMU_DIR = "${datadir}/doms"
DOMU_CFG_SRC = "${WORKDIR}/domu-conf.sample"

do_install() {
    install -d ${D}${DOMU_DIR}
    install -d ${D}${XEN_AUTO_DIR}

    for idx in $(seq 1 ${DOMU_INSTANCES})
    do
        install -Dm 0644 ${DOMU_TMPDIR}/deploy/images/${DOMU_MACHINE}/${DOMU_KERNEL_IMG} \
        ${D}${DOMU_DIR}/Image-domu${idx}.bin

        install -Dm 0644 ${DOMU_TMPDIR}/deploy/images/${DOMU_MACHINE}/domu${idx}-image-${DOMU_MACHINE}.wic.qcow2 \
        ${D}${DOMU_DIR}/domu${idx}.wic.qcow2

        DOMU_HOSTNAME=$(grep -oP "(?<=DOMU${idx}_HOSTNAME=\")[^\"]*" \
                      ${DOMU_ENV_PATH}/domus.env)

        DOMU_MEMORY_SIZE=$(grep -oP "(?<=DOMU${idx}_MEMORY_SIZE=\")[^\"]*" \
                         ${DOMU_ENV_PATH}/domus.env)
        DOMU_NUMBER_OF_CPUS=$(grep -oP "(?<=DOMU${idx}_NUMBER_OF_CPUS=\")[^\"]*" \
                           ${DOMU_ENV_PATH}/domus.env)

        export DOMU_HOSTNAME="${DOMU_HOSTNAME}"
        export DOMU_MEMORY_SIZE="${DOMU_MEMORY_SIZE}"
        export DOMU_NUMBER_OF_CPUS="${DOMU_NUMBER_OF_CPUS}"

        export DOMU_DIR="${datadir}/doms"
        export DOMU_KERNEL_IMG="${DOMU_DIR}/Image-domu${idx}.bin"
        export DOMU_DISK_DST="${DOMU_DIR}/domu${idx}.wic.qcow2"
        envsubst < ${DOMU_CFG_SRC} > "${D}${XEN_AUTO_DIR}/domu${idx}.cfg"

        # Extra configuration for domu idx can be set by DOMU{idx}_EXTRA
        if [ -e "${DOMU_ENV_PATH}/domu${idx}-extra.cfg" ]; then
            cat "${DOMU_ENV_PATH}/domu${idx}-extra.cfg" >> "${D}${XEN_AUTO_DIR}/domu${idx}.cfg"
        fi
    done
}

do_install[mcdepends] += "mc::domu:domu1-image:do_image_complete"
do_install[mcdepends] += "${@ 'mc::domu:domu2-image:do_image_complete' if d.getVar('DOMU_INSTANCES') == '2' else '' }"
do_install[prefuncs] += "gen_domu_env_and_extra_conf"

FILES:${PN} = "${DOMU_DIR} ${XEN_AUTO_DIR}"

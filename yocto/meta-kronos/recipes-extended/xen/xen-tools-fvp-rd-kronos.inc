#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

do_install:append(){
    # We want to make sure that the domain will be "dead" when "xl shutdown" is called
    sed -i 's#XENDOMAINS_SHUTDOWN="--wait"#XENDOMAINS_SHUTDOWN="--wait --wait"#' ${D}${sysconfdir}/default/xendomains
}


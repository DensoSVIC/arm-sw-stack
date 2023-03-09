# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# This file centralizes the variables and links used throughout the
# documentation. The dictionaries are converted to a single string that is used
# as the rst_prolog (see the Sphinx Configuration documentation at
# [inclusivity-exception]
# https://www.sphinx-doc.org/en/master/usage/configuration.html for more info).

# There are two types of key-value substitutions:
#     1. simple string replacements
#     2. replacement with a rendered hyperlink, where the key defines what the
#        rendered hyperlink text will be

# Prepend the key with "link:" to identify it as a Sphinx target name for use
# as a hyperlink. The "link:" prefix is dropped from the substitution name.
#
# For example:
#   "link:This URL": "www.arm.com"
#   "company name": "arm"
# Can be used as:
#   The |company name| website can be found at |This URL|_.
#
# Note the "_" which renders the substitution as a hyperlink is only possible
# because the variable is defined as a link, to be resolved as a Sphinx target.

yocto_version = "langdale"
yocto_doc_version = yocto_version + "/"
kronos_version = "main"
kas_version = "3.2"
trusted_firmware_m_version = "undefined"
trusted_firmware_m_base_version = "undefined"
scp_firmware_version = "undefined"
scp_firmware_base_version = "undefined"
trusted_firmware_a_version = "undefined"
trusted_firmware_a_base_version = "undefined"
uboot_version = "undefined"
linux_version = "undefined"
linux_version_patch = "undefined"

general_links = {
  "link:kas build tool": f"https://kas.readthedocs.io/en/{kas_version}/userguide.html",
  "link:how to install the essential packages": f"https://docs.yoctoproject.org/{yocto_doc_version}singleindex.html#required-packages-for-the-build-host",
  "link:kas Dependencies & installation": f"https://kas.readthedocs.io/en/{kas_version}/userguide.html#dependencies-installation",
  "link:EULA": f"https://developer.arm.com/downloads/-/arm-ecosystem-fvps/eula",
  "link:Writing New Tests": f"https://docs.yoctoproject.org/{yocto_doc_version}dev-manual/common-tasks.html#writing-new-tests",
  "link:testimage.bbclass": f"https://docs.yoctoproject.org/{yocto_doc_version}ref-manual/classes.html#testimage-bbclass",
  "link:OEQA FVP": f"https://git.yoctoproject.org/meta-arm/tree/documentation/oeqa-fvp.md?h={yocto_version}",
  "link:Trusted Firmware-M repository": f"https://git.trustedfirmware.org/TF-M/trusted-firmware-m.git/tree/?h={trusted_firmware_m_version}",
  "link:SCP-Firmware repository": f"https://gitlab.arm.com/arm-reference-solutions/scp-firmware/-/tree/{scp_firmware_version}",
  "link:Trusted Firmware-A repository": f"https://gitlab.arm.com/arm-reference-solutions/trusted-firmware-a/-/tree/{trusted_firmware_a_version}",
  "link:U-Boot repository": f"https://source.denx.de/u-boot/u-boot/-/tree/v{uboot_version}",
  "link:Linux repository": f"https://git.yoctoproject.org/linux-yocto/log/?h=v{linux_version}%2Fstandard%2Fbase",
}

layer_definitions = {
  "kronos remote": "https://git.gitlab.arm.com/automotive-and-industrial/kronos/kronos.git",
  "kronos version": f"{kronos_version}",
}

other_definitions = {
  "kas version": f"{kas_version}",
  "Arm": "Arm\ :sup:`®`",
  "Trusted Firmware-M version": f"{trusted_firmware_m_version}",
  "Trusted Firmware-M base version": f"{trusted_firmware_m_base_version}",
  "SCP-Firmware version": f"{scp_firmware_version}",
  "SCP-Firmware base version": f"{scp_firmware_base_version}",
  "Trusted Firmware-A version": f"{trusted_firmware_a_version}",
  "Trusted Firmware-A base version": f"{trusted_firmware_a_base_version}",
  "U-Boot version": f"{uboot_version}",
  "Linux version": f"{linux_version}.{linux_version_patch}",
}


def generate_link(key, link):

    definition = f".. _{key}: {link}"
    key_mapping = f".. |{key}| replace:: {key}"
    return f"{definition}\n{key_mapping}"


def generate_replacement(key, value):

    replacement = f".. |{key}| replace:: {value}"
    return f"{replacement}"


def generate_rst_prolog():

    rst_prolog = ""

    for variables_group in [general_links,
                            layer_definitions,
                            other_definitions]:

        for key, value in variables_group.items():
            if key.startswith("link:"):
                rst_prolog += generate_link(key.split("link:")[1], value) + "\n"
            else:
                rst_prolog += generate_replacement(key, value) + "\n"

    return rst_prolog

# Copyright (c) 2023, Arm Limited.
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

general_links = {
  "link:testimage": f"https://docs.yoctoproject.org/{yocto_doc_version}ref-manual/classes.html#testimage-bbclass",
  "link:Writing New Tests": f"https://docs.yoctoproject.org/{yocto_doc_version}dev-manual/common-tasks.html#writing-new-tests",
  "link:testimage.bbclass": f"https://docs.yoctoproject.org/{yocto_doc_version}ref-manual/classes.html#testimage-bbclass",
  "link:OEQA FVP": f"https://git.yoctoproject.org/meta-arm/tree/documentation/oeqa-fvp.md?h={yocto_version}",
}

other_definitions = {
  "Arm": "Arm\ :sup:`®`",
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
                            other_definitions]:

        for key, value in variables_group.items():
            if key.startswith("link:"):
                rst_prolog += generate_link(key.split("link:")[1], value) + "\n"
            else:
                rst_prolog += generate_replacement(key, value) + "\n"

    return rst_prolog

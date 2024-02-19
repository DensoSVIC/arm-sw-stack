<!--
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT
-->

# Kronos Reference Software Stack

To build a local version of the documentation, you will need [Sphinx][1]
installed in your work environment.

The following commands should be executed on a Linux machine and have been
tested on hosts running Ubuntu 20.04 LTS. These can be used to generate an HTML
version of the documentation under `public/`:

    sudo apt-get install python3-pip
    pip3 install -U -r documentation/requirements.txt
    sphinx-build -b html -a -W documentation public

To render and explore the documentation, simply open `public/index.html` in a
web browser.

[1]: https://www.sphinx-doc.org/

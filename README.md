<!--
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT
-->

# Arm® Automotive Solutions

The Arm Automotive Solutions Software Reference Stack is composed of multiple
Open Source components which together demonstrate use cases for the automotive
sector, including:

- The [Runtime Security Engine (RSE)][1] running an instance of Trusted
  Firmware-M, which offers boot, cryptography, and secure storage services.

- The Safety Island subsystem, running three instances of the Zephyr real-time
  operating system (RTOS).

- The firmware for the Primary Compute, using Trusted Firmware-A, U-Boot,
  OP-TEE and Trusted Services. These are configured to be aligned with
  [Arm SystemReady IR][2].

The remaining software in the Primary Compute subsystem, based on the
[EWAOL][3] distribution, is available in two main architectures:
baremetal and virtualization.

## Arm Automotive Solutions Documentation

The project's documentation can be browsed at
<https://arm-auto-solutions.docs.arm.com>.

To build a local version of the documentation, you will need [Sphinx][4]
installed in your work environment.

The following commands should be executed on a Linux machine and have been
tested on hosts running Ubuntu 20.04 LTS. These can be used to generate an HTML
version of the documentation under `public/`:

    sudo apt-get install python3-pip
    pip3 install -U -r documentation/requirements.txt
    sphinx-build -b html -a -W documentation public

To render and explore the documentation, simply open `public/index.html` in a
web browser.

# Disclaimer

Arm is a registered trademark of Arm Limited (or its subsidiaries or affiliates).

[1]: https://trustedfirmware-m.readthedocs.io/en/tf-mv2.1.0/platform/arm/rse/readme.html
[2]: https://www.arm.com/architecture/system-architectures/systemready-certification-program/ir
[3]: https://gitlab.com/soafee/ewaol/meta-ewaol/-/tree/ewaol-2.0.0
[4]: https://www.sphinx-doc.org/

# Usage

Bitbake compatibility with AppArmor Security Restrictions. Users on Ubuntu 24.04 host will need to disable AppArmor restrictions for unprivileged user namespace to effectively run bitbake. This can be done by executing the following as before starting the build process:

```
echo 0 | sudo tee /proc/sys/kernel/apparmor_restrict_unprivileged_userns
```

To begin the build, use kas:

```
kas menu
```

#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

project('si-hello-world', 'c')

app = executable('si_hello_world.elf', 'si_hello_world.S',
    link_args: ['-nostdlib'],
    install: true,
    install_dir: '/firmware',
)

objcopy = find_program('objcopy')
custom_target(
    output: ['si-hello-world.bin'],
    command: [objcopy, '--dump-section', '.text=si-hello-world.bin',
              'si_hello_world.elf'],
    depends: app,
    build_by_default: true,
    install: true,
    install_dir: '/firmware',
)

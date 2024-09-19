#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from time import sleep
from oeqa.utils.linux_terminal_utils import LinuxTermUtils


class XenUtils:

    @staticmethod
    def enter_guest_from_dom0(console, dom0_prompt, domu_prompt, domu_name):
        sleep(3)
        console.sendline()
        console.expect(dom0_prompt, timeout=45)
        console.sendline(f'xl console {domu_name}')
        console.sendline()
        console.expect(rf'{domu_name} login:', timeout=1500)
        console.sendline('root')
        console.expect(domu_prompt, timeout=30)
        # The xl console has, by default, 20 lines and 80 columns set. This
        # makes the terminal output a \x1b[k escape sequence when outputting
        # long strings. Change the column number so that this sequence does
        # not interfere with long expressions used in expect() calls.
        console.sendline('stty rows 76 cols 282')
        console.expect(r'stty rows 76 cols 282', timeout=45)
        # Clean the console state from eventual garbage
        console.sendline()
        console.expect(domu_prompt, timeout=30)

    @staticmethod
    def exit_guest_to_dom0(console, dom0_prompt, domu_prompt, domu_name,
                           telnet=True):
        sleep(3)
        # Return to Dom0

        # Stop any running command with ctrl-C
        console.sendcontrol('C')
        console.sendline()
        console.expect(domu_prompt, timeout=45)
        # Send an "exit" to logout from the guest
        console.sendline('exit')
        console.expect('exit', timeout=90)
        console.expect(rf'{domu_name} login:', timeout=450)
        # Send Ctrl-] to exit 'xl console' or to enter telnet console
        console.sendcontrol(']')
        if telnet:
            console.expect(r'telnet>', timeout=45)
            console.sendline(r'send esc')
        else:
            console.sendline()
        console.expect(dom0_prompt, timeout=90)

    @staticmethod
    def spawn_console_domu(linux_prompt, dom0_prompt, domu_hostname, target,
                           logger):
        linux_console = LinuxTermUtils.open_ssh_shell(target, domu_hostname,
                                                      logger)
        XenUtils.enter_guest_from_dom0(linux_console, dom0_prompt,
                                       linux_prompt, domu_hostname)

        return linux_console

    @staticmethod
    def close_console_domu(linux_console, linux_prompt, dom0_prompt,
                           domu_hostname, logger):
        XenUtils.exit_guest_to_dom0(linux_console.console, dom0_prompt,
                                    linux_prompt, domu_hostname, False)
        LinuxTermUtils.close_ssh_shell(linux_console.console, logger)

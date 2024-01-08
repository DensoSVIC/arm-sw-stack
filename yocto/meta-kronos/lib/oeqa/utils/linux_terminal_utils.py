#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

import os
import pexpect
import re
from datetime import datetime
from time import sleep


class LinuxBackgroundRun(object):
    class LinuxBgException(Exception):
        pass

    def __init__(self, lt_utils, cmd, timeout=None):
        self.bin_name = os.path.basename(cmd.split(" ")[0])
        dt_string = datetime.now().strftime("%Y%m%d%H%M%S")
        self.cmd_log = f"/tmp/{self.bin_name}-{dt_string}.log"
        self.cmd = cmd + f" &>{self.cmd_log} &"
        self.lt_utils = lt_utils
        self.console = self.lt_utils.console
        self.logger = self.lt_utils.logger
        self.timeout = self.lt_utils.timeout if timeout is None else timeout
        self.cmd_output = ''

    def __enter__(self):
        self.run()
        return self.console

    def __exit__(self, exc_type, exc_val, exc_tb):
        if self.kill() and exc_type is None and exc_val is None \
           and exc_tb is None:
            return True

        # An error occurred in the context
        return False

    def is_alive(self):
        # Sending signal zero checks the existence of the process and it's safe
        # to be used because we have permission on the process that is spawned
        status, _ = self.lt_utils.run(f'kill -s 0 {self.pid}')
        return True if status == 0 else False

    def run(self):
        self.logger.debug(f"LinuxBackgroundRun: Executing command: {self.cmd}")
        self.console.sendline(self.cmd)
        self.lt_utils.send_wait_prompt()
        # $! contains the PID of the last background process spawned
        self.console.sendline("echo $!")
        self.console.expect(
            rf'{self.lt_utils.endline}(\d+){self.lt_utils.endline}',
            timeout=self.timeout)
        self.pid = int(self.console.match[1])
        self.logger.debug(f"LinuxBackgroundRun: PID: {self.pid}")
        if not self.is_alive():
            raise self.LinuxBgException(('LinuxBackgroundRun Failed:\n'
                                         f'{self.bin_name} is not running!'))
        self.logger.debug(f"LinuxBackgroundRun: {self.bin_name} is running!")
        self.lt_utils.send_wait_prompt()

        return self.pid

    def kill(self):
        err_msg = ''
        self.logger.debug(f"LinuxBackgroundRun: Stopping {self.bin_name}")
        status, _ = self.lt_utils.run(f'kill -s INT {self.pid}')
        if status != 0:
            err_msg += f'Failed to stop \"{self.cmd}\" ({status})\n'
        sleep(2)
        if self.is_alive():
            self.lt_utils.run(f'kill -s KILL {self.pid}')
            sleep(2)
        s_cat, self.cmd_output = self.lt_utils.run(f"cat {self.cmd_log}")
        if s_cat != 0:
            err_msg += f'Unable to retrieve the logs ({self.cmd_log})'
        if err_msg != '':
            raise self.LinuxBgException(
                    f'LinuxBackgroundRun Failed:\n{err_msg}')
        self.logger.debug(
            f"LinuxBackgroundRun: {self.bin_name} logs:\n{self.cmd_output}")

        return True


class LinuxTermUtils(object):
    endline = r'\r?\r\n'
    timeout = 60

    def __init__(self, testcase, console, prompt_string):
        self.console = console
        self.prompt = prompt_string
        self.logger = testcase.logger
        # Xen console fix
        self.pexpect_send = self.console.send
        self.console.send = self.chunked_send

    def chunked_send(self, s):
        # The Xen console has a limitation where if more than 128 characters
        # are written in the console buffer in a short amount of time, let's
        # say for example a copy paste, the excess characters will be lost.
        # Pexpect uses os.write() to send the character list to the stdin and
        # it has no parameter to limit the flow.
        # So this function writes maximum 128 characters and wait for
        # some time that the buffer is cleared by the guest before sending
        # the excess.
        n = 128
        count = 0
        send_strings = [s[i:i+n] for i in range(0, len(s), n)]
        for chunk in send_strings:
            count += self.pexpect_send(chunk)
            sleep(1.5)

        return count

    @classmethod
    def open_ssh_shell(cls, oefvpsshtarget_obj, shell_name, logger=None):
        logfile = oefvpsshtarget_obj._create_logfile(shell_name)
        cmd_ssh = " ".join(oefvpsshtarget_obj.ssh + [oefvpsshtarget_obj.ip])
        if logger is not None:
            logger.debug((f"LinuxTermUtils: open_ssh_shell ({shell_name}):"
                          f" '{cmd_ssh}'"))
        shell = pexpect.spawn(f"{cmd_ssh}", logfile=logfile)
        shell.__ltu_shell_name = shell_name
        shell.sendline()
        shell.expect(rf'{cls.endline}(\w+@.*:.*[#\$])\s*{cls.endline}',
                     timeout=cls.timeout)
        shell.__ltu_shell_prompt = shell.match[1]
        if logger is not None:
            logger.debug((f"LinuxTermUtils: open_ssh_shell ({shell_name}) "
                          f"Shell open, prompt: {shell.__ltu_shell_prompt}"))

        return shell

    @classmethod
    def close_ssh_shell(cls, shell, logger=None):
        # Stop any running command with ctrl-C
        shell.sendcontrol('C')
        shell.sendline()
        shell.expect(shell.__ltu_shell_prompt, timeout=cls.timeout)
        try:
            shell.sendline("exit")
            sleep(1)
            shell.kill(9)
            shell.wait()
        except pexpect.EOF:
            pass
        if logger is not None:
            logger.debug(("LinuxTermUtils: close_ssh_shell: "
                          f"Shell '{shell.__ltu_shell_name}' closed"))

    def send_wait_prompt(self):
        self.console.sendline()
        self.console.expect(self.prompt, timeout=self.timeout)

    def run(self, cmd, timeout=None):
        # This function saves the exit code from the "cmd" and outputs
        # a pattern at the end so that expect can look for that pattern.
        # Using this function there is no need to know the cmd output and
        # expect will wait for it anyway thanks to the pattern.
        tout = self.timeout if timeout is None else timeout
        magic_pattern = r'--<#>--'
        self.logger.debug(f"LinuxTermUtils: Executing command: {cmd}")
        self.console.sendline(cmd + f';errcode=$?;echo \"{magic_pattern}\"')
        # First expect to wait at least the command in echo
        self.console.expect(f'{cmd.split(" ")[0]}')
        # Second expect to wait for the pattern
        self.console.expect(rf'{self.endline}{magic_pattern}{self.endline}',
                            timeout=tout)
        # The output is between the 1st and 2nd expect, start removing the \r
        # followed by any other character, that happens when the shell column
        # are not enough and so a carriage return and the character immediately
        # before it are sent again in the shell, this needs to be removed
        before = re.sub(rb'\r[^\n\r]', b'', self.console.before)
        # xl console output shows a double '\r' that appears weird in the
        # output, convert it into a single, it's safe to apply regardless of if
        # we are using xl console or not.
        before = re.sub(rb'\r\r', b'\r', before)
        # Convert from byte array to string of characters
        output = before.decode("utf-8", errors="replace").strip()
        # Remove the command echo until the pattern from the output
        output = re.sub(rf'.*{magic_pattern}\"({self.endline})?', '', output)
        self.logger.debug(f"LinuxTermUtils: Output:\n{output}")
        self.console.sendline('echo $errcode')
        self.console.expect(rf'{self.endline}(\d+){self.endline}',
                            timeout=tout)
        exit_code = int(self.console.match[1])
        self.logger.debug(f"LinuxTermUtils: Return code: {exit_code}")
        self.send_wait_prompt()

        return (exit_code, output)

    def background_cmd_ctx(self, cmd, timeout=None):
        return LinuxBackgroundRun(self, cmd, timeout)

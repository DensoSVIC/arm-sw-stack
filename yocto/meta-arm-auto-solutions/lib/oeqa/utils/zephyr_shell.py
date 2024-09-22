#
# Based on: https://github.com/zephyrproject-rtos/zephyr/blob/v3.5.0/scripts/pylib/pytest-twister-harness/src/twister_harness/helpers/shell.py  # noqa: E501
# In open-source project: Zephyr
#
# Original file: SPDX-FileCopyrightText: <text>Copyright (c) 2023 Nordic
# Semiconductor ASA</text>
# Modifications: SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited
# and/or its affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: Apache-2.0

# Changes:
# 1) Adaptations to use pexpect instead of Zephyr's DeviceAdapter
# 2) exec_command changed to return a single string instead of a list of lines,
#    for consistency with OEQA
# 3) Remove type hint syntax only supported by Python >= 3.10
# 4) Increase timeout of echo read-back

import pexpect
import re
import time


class Shell:
    def __init__(self, target, console, logger, prompt: str = 'uart:~$'):
        self.target = target
        self.console = console
        self.logger = logger
        self.prompt = prompt
        self.base_timeout: float = 30.0

    def clear_buffer(self):
        try:
            while True:
                # "Expect" all input until a timeout occurs
                self.target.expect(self.console, '.+', timeout=0)
        except (pexpect.TIMEOUT, pexpect.EOF):
            pass

    def wait_for_prompt(self, timeout=None) -> bool:
        """
        Send every 0.5 second "enter" command to the device until shell prompt
        statement will occur (return True) or timeout will be exceeded (return
        False).
        """
        timeout = timeout or self.base_timeout
        timeout_time = time.time() + timeout
        regex_prompt = re.escape(self.prompt)
        self.clear_buffer()
        while time.time() < timeout_time:
            self.send_empty_line()
            self.logger.debug("Awaiting console...")
            try:
                line = self.target.expect(self.console, regex_prompt,
                                          timeout=0.5)
            except pexpect.TIMEOUT:
                # ignore read timeout and try to send enter once again
                continue
            self.logger.debug('Got prompt')
            time.sleep(0.05)
            self.clear_buffer()
            return True
        return False

    def run_command(self, command: str):
        regex_command = f'.*{command}'
        self.logger.debug(f"Executing command: {command}")
        self.target.sendline(self.console, command.encode())
        # wait for device command print - it should be done immediately after
        # sending command to device
        self.target.expect(self.console, regex_command, timeout=20.0)

    def exec_command(self, command: str, timeout=None) -> str:
        """
        Send shell command to a device and return response. The call to
        run_command expects the command string itself, meaning that the console
        output returned is for the execution of the command.
        """
        lines, _ = self.exec_fn(
            timeout_prompt=timeout, fn=self.run_command, command=command)

        return lines

    def exec_fn(self, fn, *args, timeout_prompt=None, **kwargs):
        """
        Call any function and return a tuple of:
        1. The console output during the time of the function run.
        2. The function return value.
        """
        timeout_prompt = timeout_prompt or self.base_timeout
        self.clear_buffer()
        self.logger.debug(f"Calling monitored function")
        fn_return = fn(*args, **kwargs)
        # wait for device command execution
        regex_prompt = re.escape(self.prompt)
        self.target.expect(self.console, regex_prompt,
                           timeout=timeout_prompt)
        lines = self.target.before(self.console).strip().decode(
            "utf-8", errors="replace")
        self.logger.debug(f"Output:\n{lines}")
        return lines, fn_return

    def send_empty_line(self):
        self.target.sendline(self.console)

#
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

from oeqa.core.decorator.depends import OETestDepends
from oeqa.runtime.case import OERuntimeTestCase
import os
import re
import time


class SystemReadyACSCapsuleUpdateTest(OERuntimeTestCase):

    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        cls.tc.target.transition('on')
        cls.log = ""
        cls.test_fs = "FS1:"
        cls.cap_fs = "FS0:"
        cls.capsule = "fw.cap"
        cls.shell_prompt = fr"{cls.test_fs}.*>"
        cls.rse_console = "rse"
        cls.acs_console = cls.td.get('ARM_SYSTEMREADY_ACS_CONSOLE')
        cls.log_filename = "capsule-update.log"
        cls.def_timeout = 150

    @classmethod
    def tearDownClass(cls):
        cls.tc.target.transition('off')
        super().tearDownClass()

    def strip_output(self, output):
        # Remove UEFI shell escape sequences for the colors
        log = re.sub(br'\x1b\[[\d;]+[mH]', b'', output)
        return log.decode("utf-8", errors="replace").strip()

    def run_capsule_app(self, capsule_name, dump_header_info=False,
                        wait_prompt=True):
        dump_flag = ""
        if dump_header_info:
            dump_flag = "-D "
        self.send_command_chunked(f"CapsuleApp.efi {dump_flag} "
                                  f"{capsule_name}\r")
        if wait_prompt:
            self.target.expect(self.acs_console, self.shell_prompt,
                               timeout=self.def_timeout)

    def save_log(self):
        log = self.strip_output(self.target.before(self.acs_console))
        self.log += log + "\n"
        return log

    def enter_uefi_shell(self):
        ESC = "\x1b"
        self.target.expect(self.acs_console,
                           r"The highlighted entry will be executed "
                           r"automatically in .*s.",
                           timeout=self.def_timeout)
        self.save_log()
        self.target.sendline(self.acs_console, r'\r')
        self.target.expect(self.acs_console, r"Press .* in 5 seconds to skip",
                           timeout=self.def_timeout)
        self.save_log()
        self.target.expect(self.acs_console, "or any other key to continue.",
                           timeout=self.def_timeout)
        self.save_log()
        self.target.send(self.acs_console, ESC)
        self.target.expect(self.acs_console, ESC, timeout=self.def_timeout)
        self.save_log()
        self.target.expect(self.acs_console, "Shell>",
                           timeout=self.def_timeout)
        self.save_log()
        self.target.sendline(self.acs_console, f"{self.test_fs}\r")
        self.target.expect(self.acs_console, self.shell_prompt,
                           timeout=self.def_timeout)
        self.save_log()
        self.target.sendline(self.acs_console, f"cd EFI/BOOT/app\r")
        self.target.expect(self.acs_console, self.shell_prompt,
                           timeout=self.def_timeout)
        self.save_log()

    def send_command_chunked(self, s):
        # EDK2 has a limitation for which no more than 33 chars can be written
        # in the shell in one go, experimentally tested by copy pasting text
        # in the shell. pexpect does the same by writing into the console.
        n = 33
        send_strings = (s[i:i+n] for i in range(0, len(s), n))
        for chunk in send_strings:
            self.target.sendline(self.acs_console, chunk)
            time.sleep(0.5)

    def unsigned_capsule_update(self):
        # Copy the unsigned capsule at first
        self.send_command_chunked(
            f"cp {self.cap_fs}unsigned_{self.capsule} ./\r")
        self.target.expect(self.acs_console, self.shell_prompt,
                           timeout=self.def_timeout)

        # Attempt to update with unsigned capsule, expect failure
        self.run_capsule_app(f"unsigned_{self.capsule}", dump_header_info=True)
        self.save_log()
        self.run_capsule_app(f"unsigned_{self.capsule}")
        cmd_log = self.save_log()
        self.assertIn("failed to update capsule - Security Violation",
                      cmd_log,
                      msg="Log file does not contain failed update message")

        # Remove the capsule to save the disk space
        self.send_command_chunked(f"rm unsigned_{self.capsule}\r")
        self.target.expect(self.acs_console, self.shell_prompt,
                           timeout=self.def_timeout)
        self.save_log()

    def tampered_capsule_update(self):
        # Copy the tampered capsule at first
        self.send_command_chunked(
            f"cp {self.cap_fs}tampered_{self.capsule} ./\r")
        self.target.expect(self.acs_console, self.shell_prompt,
                           timeout=self.def_timeout)

        # Attempt to update with tampered capsule, expect failure
        self.run_capsule_app(f"tampered_{self.capsule}", dump_header_info=True)
        self.save_log()
        self.run_capsule_app(f"tampered_{self.capsule}")
        cmd_log = self.save_log()
        self.assertIn("failed to update capsule - Security Violation",
                      cmd_log,
                      msg="Log file does not contain failed update message")

        # Remove the capsule to save the disk space
        self.send_command_chunked(f"rm tampered_{self.capsule}\r")
        self.target.expect(self.acs_console, self.shell_prompt,
                           timeout=self.def_timeout)
        self.save_log()

    def signed_capsule_update(self):
        # Copy the signed capsule at first
        self.send_command_chunked(
            f"cp {self.cap_fs}{self.capsule} ./\r")
        self.target.expect(self.acs_console, self.shell_prompt,
                           timeout=self.def_timeout)

        # Update with signed capsule
        self.run_capsule_app(f"{self.capsule}", dump_header_info=True)
        self.save_log()

        # This command will reboot the target
        self.run_capsule_app(f"{self.capsule}", wait_prompt=False)
        # The RSE console will tell if the update succeeded or not
        idx = self.target.expect(self.rse_console,
                                 [r"Flashing the image succeeded",
                                  r"Flashing the image Failed"],
                                 timeout=1800)
        self.assertEqual(idx, 0, msg='Capsule update failed!')

    @OETestDepends(['arm_systemready_ir_acs_shutdown.'
                    'SystemReadyACSShutdownTest.test_shutdown'])
    def test_capsule_update(self):
        test_log_file_host = os.path.join(self.td.get('TEST_LOG_DIR'),
                                          self.log_filename)

        self.assertNotEqual(self.acs_console, '',
                            msg='ARM_SYSTEMREADY_ACS_CONSOLE is not set')

        self.enter_uefi_shell()

        self.unsigned_capsule_update()

        self.tampered_capsule_update()

        self.signed_capsule_update()

        # The target is rebooting, so wait for the start of U-Boot to handle
        # the logs for the capsule update part, we cannot redirect to a file
        # in that case because of the target reboot
        self.target.expect(self.acs_console, "U-Boot", timeout=1800)
        self.save_log()

        self.enter_uefi_shell()

        # Final command to be tested in order to export the ESRT table status
        self.send_command_chunked(f"CapsuleApp.efi -E\r")
        self.target.expect(self.acs_console, self.shell_prompt,
                           timeout=self.def_timeout)
        self.save_log()

        # Write the log into an host log file
        with open(test_log_file_host, "w") as host_file:
            host_file.write(self.log)

        self.logger.info('Capsule update test succeeded')

        # Reset the target and stop FVP
        self.send_command_chunked(f"reset\r")
        self.target.expect(self.acs_console, "U-Boot", timeout=1800)

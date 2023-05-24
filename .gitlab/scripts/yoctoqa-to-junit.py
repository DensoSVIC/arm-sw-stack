# Copyright (c) 2023 Arm Limited or its affiliates. All rights reserved.
#
# SPDX-License-Identifier: MIT

# Converts yoctoqa results to junit format

import argparse
import glob
import re

from junit_xml import TestSuite, TestCase


def main():
    parser = argparse.ArgumentParser(
        description='Converts yoctoqa results to junit format')

    parser.add_argument("log_files", help="bitbake cooker log file(s)")
    parser.add_argument("output_file", help="Output Junit log file")
    parser.add_argument(
        "-n", "--name",
        action="store",
        default="Yoctoqa test",
        help="Test Suite Name to be used in the ouput file")

    args = parser.parse_args()

    parseData(args.log_files, args.output_file, args.name)


def parseMatch(match):
    testclass = match.group(1) + '.' + match.group(2)
    testname = match.group(3)
    testresult = match.group(4)
    testtime = float(match.group(5))

    testcase = TestCase(testname,
                        elapsed_sec=testtime,
                        classname=testclass)

    if testresult == 'SKIPPED':
        testcase.add_skipped_info("skipped")
    elif testresult != 'PASSED':
        testcase.add_failure_info("failed")

    return testcase


def parseLines(lines):
    regex = r'^RESULTS\s-\s(\w+)\.(\w+)\.(\w+):\s(\w+)\s\(([\d\.]+)s\)$'
    for line in lines:
        match = re.match(regex, line)
        if match:
            yield parseMatch(match)


def parseFiles(log_file_paths):
    for log_file_path in glob.glob(log_file_paths):
        with open(log_file_path, "r") as file:
            yield from parseLines(file.readlines())


def parseData(log_file_paths, export_file, name):
    testsuites = list(parseFiles(log_file_paths))

    ts = TestSuite(name, testsuites)

    with open(export_file, 'w') as f:
        TestSuite.to_file(f, [ts])


if __name__ == '__main__':
    main()

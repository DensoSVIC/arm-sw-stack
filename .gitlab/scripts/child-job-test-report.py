# Copyright (c) 2023 Arm Limited or its affiliates. All rights reserved.
#
# SPDX-License-Identifier: MIT

# Generates JUnit report for all child jobs

import json
import sys
import requests

from envparse import Env
from junit_xml import TestSuite, TestCase


def get_response(url, headers):
    """
    Call the GitLab API and return the json formated result
    """
    response = requests.get(url, headers=headers)
    if not response.ok:
        try:
            generic_message = response.json().get('message', generic_message)
        except json.decoder.JSONDecodeError:
            generic_message = 'Invalid response from GitLab'
        sys.stderr.write(generic_message + '\n')
        exit(1)
    return response.json()


def get_test_results(headers, project_url, pipeline_id):
    """
    fetch a pipelines test report
    """
    report_url = f'{project_url}/pipelines/{pipeline_id}/test_report'

    return get_response(f'{report_url}', headers)


def generate_junit_file(test_results, filename):
    """
    Generate a Junit file from pipeline test results
    """
    tss = []
    for test_suite in test_results['test_suites']:
        tcs = []
        for test_case in test_suite['test_cases']:
            tc = TestCase(test_case['name'],
                          elapsed_sec=test_case['execution_time'],
                          classname=test_suite['name'] +
                          "." + test_case['classname'],
                          stdout=test_case['system_output'])

            if test_case['status'] == 'error':
                tc.add_error_info("error")
            elif test_case['status'] == 'failed':
                tc.add_failure_info("failed")
            elif test_case['status'] == 'skipped':
                tc.add_skipped_info("skipped")

            tcs.append(tc)

        tss.append(TestSuite(test_suite['name'], tcs))

    with open(filename, 'w') as f:
        TestSuite.to_file(f, tss)


def main():
    """
    download artifacts and place them in the jobs directory
    """
    env = Env()

    v4_origin = env('CI_API_V4_URL')
    project_id = env("CI_PROJECT_ID")
    pipeline_id = env('CI_PIPELINE_ID')
    headers = {
        'PRIVATE-TOKEN': env('PRIVATE_CI_TOKEN')
    }

    project_url = f'{v4_origin}/projects/{project_id}'
    page_url = f'{project_url}/pipelines/{pipeline_id}/bridges/'
    json_response = get_response(f'{page_url}', headers)

    for bridge in json_response:
        if bridge['downstream_pipeline']:
            test_results = get_test_results(headers,
                                            project_url,
                                            bridge['downstream_pipeline']['id']
                                            )

            generate_junit_file(test_results, "TEST-ChildPipeline-" +
                                str(bridge['downstream_pipeline']['id']) +
                                ".xml")


if __name__ == '__main__':
    main()

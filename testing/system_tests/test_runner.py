#!/usr/bin/env python3
import os
import logging
from argparse import ArgumentParser

from internals import logger_config
from internals.test import Test

TESTS_DATA_PATH = './scenarios'

LOGGER = logging.getLogger("test_runner")


def parse_args():
    parser = ArgumentParser("System tests runner")
    parser.add_argument('test_name', metavar='TEST_NAME', nargs='?')
    parser.add_argument(
        '-l', '--list', action='store_true', help="List all tests names")
    parser.add_argument(
        '-v', '--verbose', action='store_true', help="Show more logs from running tests")
    parser.add_argument(
        '-p', '--executable-path', help="Specify scheduler executable path")
    parser.add_argument(
        '-i', '--input-dir', default='./run', help="Specify scheduler input directory")
    return parser.parse_args()


def get_all_tests_names():
    return [f for f in os.listdir(TESTS_DATA_PATH)]


def run_single_test(test_name, lss_input_dir):
    LOGGER.info("Test: %s ... ", test_name)
    test = Test(TESTS_DATA_PATH, test_name, args.lss_input_dir, args.executable_path)
    test.run()
    if test.has_failed:
        LOGGER.error("Test: %s ... FAILED", test_name)
    else:
        LOGGER.info("Test: %s ... PASSED", test_name)


def run_all_tests(lss_input_dir):
    for test_name in get_all_tests_names():
        run_single_test(test_name, lss_input_dir)
        LOGGER.info(60 * '=')


if __name__ == "__main__":
    args = parse_args()
    logger_config.setup(args.verbose)

    if args.list:
        for test_name in get_all_tests_names():
            print(test_name)
        exit(0)

    if args.test_name:
        run_single_test(args.test_name, args.lss_input_dir)
    else:
        run_all_tests(args.lss_input_dir)

#!/usr/bin/env python3
import os
import logging
from argparse import ArgumentParser

from internals.logger_config import LoggerConfig
from internals.test import Test

TESTS_DATA_PATH = './scenarios/'

LOGGER = logging.getLogger("test_runner")


def parse_args():
    parser = ArgumentParser(description="System tests runner")
    parser.add_argument('scheduler',
                        metavar='SCHEDULER',
                        help="The scheduler executable path")
    parser.add_argument('test_name',
                        metavar='TEST_NAME',
                        nargs='?',
                        help="The name of a test to run. If it was not specified, all test will be run")
    parser.add_argument('-l', '--list',
                        action='store_true',
                        help="List all tests names")
    parser.add_argument('-v', '--verbose',
                        action='store_true',
                        help="Show more logs from running tests")
    parser.add_argument('-i', '--input-dir',
                        default='.',
                        help="Specify scheduler input directory (default .)")
    return parser.parse_args()


def get_all_tests_names():
    return sorted([f for f in os.listdir(TESTS_DATA_PATH)])


def run_single_test(test_name, lss_input_dir, scheduler_path):
    with LoggerConfig.test_file_handler(test_name) as test_log_dir:
        LOGGER.info("Test: %s ... ", test_name)
        test = Test(
            TESTS_DATA_PATH,
            test_name,
            lss_input_dir,
            scheduler_path,
            test_log_dir
        )
        test.run()
        if test.has_failed():
            LOGGER.error("Test: %s ... FAILED", test_name)
        else:
            LOGGER.info("Test: %s ... PASSED", test_name)


def run_all_tests(lss_input_dir, scheduler_path):
    for test_name in get_all_tests_names():
        run_single_test(test_name, lss_input_dir, scheduler_path)
        LOGGER.info(60 * '=')


if __name__ == "__main__":
    args = parse_args()
    LoggerConfig.setup(args.test_name, args.verbose)

    if args.list:
        for test_name in get_all_tests_names():
            print(test_name)
        exit(0)

    scheduler_path = os.path.join(os.curdir, args.scheduler)
    if args.test_name:
        run_single_test(args.test_name, args.input_dir, scheduler_path)
    else:
        run_all_tests(args.input_dir, scheduler_path)

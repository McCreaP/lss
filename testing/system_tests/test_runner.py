#!/usr/bin/env python3
import os
import time
import logging
from argparse import ArgumentParser

from internals import utils
from internals import logger_config
from internals.test import Test

TESTS_DATA_PATH = './scenarios/'

LOG_ROOT_DIR = './logs/'

LOGGER = logging.getLogger("test_runner")


def parse_args():
    parser = ArgumentParser(description="System tests runner")
    parser.add_argument('scheduler',
                        metavar='SCHEDULER',
                        nargs=1,
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
    return [f for f in os.listdir(TESTS_DATA_PATH)]


def make_log_dir(test_name):
    log_dir = os.path.join(
        LOG_ROOT_DIR, test_name + '_' + utils.human_readable_time(time.time()))
    os.makedirs(log_dir, mode=0o775)
    return log_dir


def run_single_test(test_name, lss_input_dir, scheduler_path):
    log_dir = make_log_dir(test_name)
    logger_config.add_file_handler(log_dir)
    LOGGER.info("Test: %s ... ", test_name)
    test = Test(
        TESTS_DATA_PATH, test_name, lss_input_dir, scheduler_path, log_dir)
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
    logger_config.setup(args.verbose)

    if args.list:
        for test_name in get_all_tests_names():
            print(test_name)
        exit(0)

    scheduler_path = os.path.join(os.curdir, args.scheduler)
    if args.test_name:
        run_single_test(args.test_name, args.input_dir, scheduler_path)
    else:
        run_all_tests(args.input_dir, scheduler_path)

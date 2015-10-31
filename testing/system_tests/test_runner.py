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
        '-l', '--list', action='store_true', help='List all tests names')
    parser.add_argument(
        '-v', '--verbose', action='store_true', help='Show more logs from running tests')
    return parser.parse_args()


def get_all_tests():
    return [f for f in os.listdir(TESTS_DATA_PATH)]

if __name__ == "__main__":
    args = parse_args()
    logger_config.setup(args.verbose)

    if args.list:
        for f in get_all_tests():
            print(f)
        exit(0)

    if args.test_name:
        Test(TESTS_DATA_PATH, args.test_name).run()
    else:
        for test_name in get_all_tests():
            Test(TESTS_DATA_PATH, test_name).run()

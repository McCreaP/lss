#!/usr/bin/env python3
import os
import logging
from argparse import ArgumentParser

import pickle
import yaml

from generators.story_generator import StoryGenerator
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
    names = set()
    for name in os.listdir(TESTS_DATA_PATH):
        if name.startswith("."):
            continue
        names.add(os.path.splitext(name)[0])
    return sorted(list(names))


def generate_story_from_yaml(test_data_path):
    with open(test_data_path + ".yaml", "r") as f:
        test_config = yaml.load(f)
    del test_config["description"]
    story = StoryGenerator(**test_config).dump()
    with open(test_data_path, "wb") as f:
        pickle.dump(story, f)


def run_single_test(test_name, lss_input_dir, scheduler_path):
    with LoggerConfig.test_file_handler(test_name) as test_log_dir:
        test_data_path = os.path.join(TESTS_DATA_PATH, test_name)
        if os.path.exists(test_data_path):
            pass
        elif os.path.exists(test_data_path + ".yaml"):
            LOGGER.info("Test: %s WILL BE GENERATED AUTOMATICALLY ", test_name)
            generate_story_from_yaml(test_data_path)
            LOGGER.info("Test: %s GENERATION FINISHED ", test_name)
        else:
            LOGGER.error("Test: %s NOT FOUND", test_name)
            return
        LOGGER.info("Test: %s ... ", test_name)
        test = Test(test_data_path,
                    lss_input_dir,
                    scheduler_path,
                    test_log_dir)
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

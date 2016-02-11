import os
import shutil
import itertools
import datetime
import logging

LOGGER = logging.getLogger('test_runner')


def flatten(lst):
    return list(itertools.chain(*lst))


def make_empty_dir(dir_path):
    LOGGER.debug("Make empty dir: %s", dir_path)
    if os.path.exists(dir_path):
        shutil.rmtree(dir_path)
    make_dir(dir_path)


def make_dir(dir_path):
    os.makedirs(dir_path, mode=0o775)


def human_readable_time(timestamp):
    return datetime.datetime.fromtimestamp(timestamp).strftime('%d-%m-%Y_%H-%M-%S')

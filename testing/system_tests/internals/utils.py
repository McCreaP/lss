import os
import shutil
import itertools
import datetime
import logging

LOGGER = logging.getLogger('test_runner')


def flatten(lst):
    return list(itertools.chain(*lst))


def clean_dir(dir_path):
    if os.path.exists(dir_path):
        shutil.rmtree(dir_path)
        LOGGER.debug("Removed %s", dir_path)


def remove_file(filename):
    if os.path.exists(filename):
        os.remove(filename)
        LOGGER.debug("Removed %s", filename)


def human_readable_time(timestamp):
    return datetime.datetime.fromtimestamp(timestamp).strftime('%d-%m-%Y_%H:%M:%S')

import os
from colorlog import ColoredFormatter
import logging

LOGGER = logging.getLogger("test_runner")

COLOR_LOG_FORMAT = "%(log_color)s%(levelname)-8s%(reset)s | %(asctime)s | %(log_color)s%(message)s%(reset)s"
MONOCHROMATIC_LOG_FORMAT = "%(levelname)-8s | %(asctime)s | %(message)s"


def setup(verbose=False):
    LOGGER.setLevel(logging.DEBUG)
    handler = logging.StreamHandler()
    if verbose:
        handler.setLevel(logging.DEBUG)
    else:
        handler.setLevel(logging.INFO)

    formatter = ColoredFormatter(COLOR_LOG_FORMAT)
    handler.setFormatter(formatter)

    LOGGER.addHandler(handler)


def add_file_handler(log_dir):
    log_file = os.path.join(log_dir, 'test_runner.log')
    handler = logging.FileHandler(log_file)
    handler.setLevel(logging.DEBUG)

    formatter = logging.Formatter(MONOCHROMATIC_LOG_FORMAT)
    handler.setFormatter(formatter)

    LOGGER.addHandler(handler)

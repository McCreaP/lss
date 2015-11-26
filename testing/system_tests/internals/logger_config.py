import os
import time
from colorlog import ColoredFormatter
import logging
from contextlib import contextmanager

from internals import utils

LOG_ROOT_DIR = './logs/'

LOGGER = logging.getLogger("test_runner")

COLOR_LOG_FORMAT = "%(log_color)s%(levelname)-8s%(reset)s | %(asctime)s | %(log_color)s%(message)s%(reset)s"
MONOCHROMATIC_LOG_FORMAT = "%(levelname)-8s | %(asctime)s | %(message)s"


class LoggerConfig:

    @classmethod
    def setup(cls, test_name, verbose=False):
        LOGGER.setLevel(logging.DEBUG)
        formatter = ColoredFormatter(COLOR_LOG_FORMAT)

        readable_time = utils.human_readable_time(time.time())
        log_dir = (test_name if test_name else 'all') + '_' + readable_time
        cls.__log_path = os.path.join(LOG_ROOT_DIR, log_dir)
        utils.make_dir(cls.__log_path)

        file_handeler = logging.FileHandler(os.path.join(cls.__log_path, 'test_runner.log'))
        file_handeler.setLevel(logging.DEBUG)
        file_handeler.setFormatter(formatter)

        stream_handler = logging.StreamHandler()
        if verbose:
            stream_handler.setLevel(logging.DEBUG)
        else:
            stream_handler.setLevel(logging.INFO)
        stream_handler.setFormatter(formatter)

        LOGGER.addHandler(stream_handler)

    @classmethod
    @contextmanager
    def test_file_handler(cls, test_name):
        test_log_dir = os.path.join(cls.__log_path, test_name)
        utils.make_dir(test_log_dir)
        log_file = os.path.join(test_log_dir, 'test_runner.log')

        handler = logging.FileHandler(log_file)
        handler.setLevel(logging.DEBUG)

        formatter = ColoredFormatter(COLOR_LOG_FORMAT)
        handler.setFormatter(formatter)

        LOGGER.addHandler(handler)
        try:
            yield test_log_dir
        finally:
            LOGGER.removeHandler(handler)

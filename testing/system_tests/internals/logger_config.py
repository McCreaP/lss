from colorlog import ColoredFormatter
import logging

LOGGER = logging.getLogger("test_runner")

LOG_FORMAT = "%(log_color)s%(levelname)-8s%(reset)s | %(asctime)s | %(log_color)s%(message)s%(reset)s"

def setup(verbose=False):
    if verbose:
        LOGGER.setLevel(logging.DEBUG)
    else:
        LOGGER.setLevel(logging.INFO)

    formatter = ColoredFormatter(LOG_FORMAT)

    handler = logging.StreamHandler()
    handler.setFormatter(formatter)

    LOGGER.addHandler(handler)
    
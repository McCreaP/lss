import time
import logging

LOGGER = logging.getLogger("test_runner")

dtime = 0.0


def setup(virtual_time):
    global dtime
    dtime = virtual_time - time.time()


def now():
    return time.time() + dtime

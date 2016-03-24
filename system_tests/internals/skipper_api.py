import logging

from bottle import route, run
from threading import Thread

LOGGER = logging.getLogger("test_runner")


class SkipperApi:
    def __init__(self, function):
        @route('/')
        def skip():
            LOGGER.debug("Received skip signal from scheduler")
            function()
            return "Skipped\n"

        self.__thread = Thread(target=run, kwargs={"host": "localhost", "port": "8000", "quiet": True})
        self.__thread.daemon = True
        self.__thread.start()

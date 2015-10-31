import os
import pickle
import traceback
import logging

from internals import timer
from internals.loop import EventLoop

LOGGER = logging.getLogger('test_runner')


class Test:

    def __init__(self, test_data_dir, test_name):
        self.__test_name = test_name
        test_data_path = os.path.join(test_data_dir, test_name)
        with open(test_data_path, 'rb') as f:
            self.__story = pickle.load(f)

    def run(self):
        LOGGER.info('Test: %s', self.__test_name)
        LOGGER.info(
            'Get %s jobs in the original story', len(self.__story['jobs']))
        timer.setup(self.__story['mint'])
        try:
            EventLoop(self.__story).run()
            LOGGER.info('Test: %s ... PASSED', self.__test_name)
        except Exception as e:
            traceback.print_exc()
            LOGGER.warning(e)
            LOGGER.error('Test: %s ... FAILED')

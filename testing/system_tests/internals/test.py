import os
import pickle
import traceback
import logging
import subprocess
import signal

from internals import timer
from internals.loop import EventLoop
from internals.state import State
from internals.story import Story

LOGGER = logging.getLogger('test_runner')


class Test:

    def __init__(self, test_data_dir, test_name, lss_input_dir, lss_executable_path, log_dir):
        self.__test_name = test_name
        self.__has_failed = False
        self.__lss_input_dir = lss_input_dir
        self.__lss_executable_path = lss_executable_path
        self.__log_dir = log_dir
        self.__state = None
        test_data_path = os.path.join(test_data_dir, test_name)
        with open(test_data_path, 'rb') as f:
            self.__story = Story(pickle.load(f))

    def run(self):
        self.__prepare_for_running()
        scheduler = subprocess.Popen(self.__lss_executable_path, shell=True)
        try:
            EventLoop(self.__story, self.__state).run()
            self.__state.write_history(self.__log_dir)
        except Exception as e:
            traceback.print_exc()
            LOGGER.warning(e)
            self.__has_failed = True
        finally:
            scheduler.send_signal(signal.SIGINT)

    def has_failed(self):
        return self.__has_failed

    def __prepare_for_running(self):
        self.__state = State(self.__story, self.__lss_input_dir)
        self.__has_failed = False
        timer.setup(self.__story.get_raw('mint'))

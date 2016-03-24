import os
import pickle
import traceback
import logging
import subprocess
import json

from internals import timer
from internals import utils
from internals.loop import EventLoop
from internals.objective_function.history import History
from internals.objective_function.objective_function import ObjectiveFunction
from internals.state import State
from internals.story import Story

LOGGER = logging.getLogger('test_runner')

LSS_ASSIGNMENTS_DIR = 'assignments'
LSS_INPUT_DIR = 'input'
LSS_INPUT_NAME = 'input'


class Test: #pylint: disable=R0903
    def __init__(self, test_data_path, run_dir, lss_executable_path, log_dir, verbose):
        self.has_failed = False
        self.__lss_input_dir = os.path.join(run_dir, LSS_INPUT_DIR)
        self.__lss_input_path = os.path.join(self.__lss_input_dir, LSS_INPUT_NAME)
        self.__lss_assignments_dir = os.path.join(run_dir, LSS_ASSIGNMENTS_DIR)
        self.__lss_executable_path = lss_executable_path
        self.__log_dir = log_dir
        self.__verbose = verbose
        self.__state = None
        self.result = None
        self.quasi_optimal_result = None
        with open(test_data_path, 'rb') as f:
            self.__story = Story(pickle.load(f))

    def run(self):
        self.__prepare_for_running()
        run_lss_command = "{executable} --input={input} --assignments={assignments} --verbose={verbose}".format(**{
            "executable": self.__lss_executable_path,
            "input": os.path.abspath(self.__lss_input_path),
            "assignments": os.path.abspath(self.__lss_assignments_dir),
            "verbose": self.__verbose})
        LOGGER.info("Run scheduler with the command: %s", run_lss_command)
        scheduler = subprocess.Popen(run_lss_command, shell=True)
        try:
            EventLoop(self.__story, self.__state).run()
            self.__process_result()
        except Exception as e:  # pylint: disable=W0703
            traceback.print_exc()
            LOGGER.warning(e)
            self.has_failed = True
        finally:
            if scheduler.poll():  # Scheduler has stopped
                self.has_failed = True
            else:
                scheduler.terminate()

    def __prepare_for_running(self):
        utils.make_empty_dir(self.__lss_assignments_dir)
        utils.make_empty_dir(self.__lss_input_dir)
        self.__state = State(self.__story,
                             self.__lss_input_path,
                             self.__lss_assignments_dir)
        self.has_failed = False
        timer.setup(self.__story.get_raw('mint'))

    def __process_result(self):
        self.__determine_quasi_optimal_result()

        raw_history = self.__state.gather_history()
        self.__write_history_to_files(raw_history)

        history = History(raw_history, self.__story.get_raw('jobs'))
        self.result = ObjectiveFunction(history).compute()

    def __determine_quasi_optimal_result(self):
        mint = self.__story.get_raw('mint')
        maxt = self.__story.get_raw('maxt')
        min_setup_time = min(self.__story.get_raw('context_changes').values())
        finished_jobs = []
        for job in self.__story.get_raw('jobs'):
            if mint <= job['ready'] <= maxt:
                job['real_start_time'] = job['ready']
                job['real_duration'] = min_setup_time + job['expected_duration_barring_setup']
                finished_jobs.append(job)
        raw_history = self.__state.gather_history(finished_jobs)
        history = History(raw_history, self.__story.get_raw('jobs'))
        self.quasi_optimal_result = ObjectiveFunction(history).compute()

    def __write_history_to_files(self, raw_history):
        log_file = os.path.join(self.__log_dir, 'history')
        with open(log_file, 'wb') as f:
            pickle.dump(raw_history, f)
        with open(log_file + '.json', 'wt') as f:
            json.dump(raw_history, f, indent=2)
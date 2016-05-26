import os
import pickle
import signal
import traceback
import logging
import subprocess
import json

from internals import timer
from internals import utils
from internals.loop import EventLoop
from internals.machine import MachineState
from internals.objective_function.history import History
from internals.objective_function.objective_function import ObjectiveFunction
from internals.state import State
from internals.story import Story

LOGGER = logging.getLogger('test_runner')

LSS_ASSIGNMENTS_DIR = 'assignments'
LSS_INPUT_DIR = 'input'
LSS_INPUT_NAME = 'input'


class Test: #pylint: disable=R0903
    def __init__(self, test_data_path, run_dir, lss_executable_path, algorithm, log_dir, verbose):
        self.has_failed = False
        self.__lss_input_dir = os.path.join(run_dir, LSS_INPUT_DIR)
        self.__lss_input_path = os.path.join(self.__lss_input_dir, LSS_INPUT_NAME)
        self.__lss_assignments_dir = os.path.join(run_dir, LSS_ASSIGNMENTS_DIR)
        self.__lss_executable_path = lss_executable_path
        self.__log_dir = log_dir
        self.__verbose = verbose
        self.__state = None
        self.__algorithm = algorithm
        self.result = None
        self.quasi_optimal_result = None
        with open(test_data_path, 'rb') as f:
            self.__story = Story(pickle.load(f))
        self.__state = self.__fresh_state()
        self.__machines_events = []
        self.__prepare_machines_events()
        self.__fair_sets_events = []
        self.__prepare_fair_sets_events()

    def run(self):
        self.__prepare_for_running()
        run_lss_command = "{executable} " \
                          "--input={input} " \
                          "--assignments={assignments} " \
                          "--algorithm={algorithm} " \
                          "--verbose={verbose} ".format(**{
            "executable": self.__lss_executable_path,
            "input": os.path.abspath(self.__lss_input_path),
            "assignments": os.path.abspath(self.__lss_assignments_dir),
            "algorithm": self.__algorithm,
            "verbose": self.__verbose})
        LOGGER.info("Run scheduler with the command: %s", run_lss_command)
        scheduler = subprocess.Popen(run_lss_command, shell=True, preexec_fn=os.setsid)
        try:
            EventLoop(self.__story, self.__state, self.__machines_events, self.__fair_sets_events).run()
            self.process_result()
        except Exception as e:  # pylint: disable=W0703
            traceback.print_exc()
            LOGGER.warning(e)
            self.has_failed = True
        finally:
            if scheduler.poll():  # Scheduler has stopped
                self.has_failed = True
            else:
                # Simple scheduler.terminate() does not work
                # see: http://stackoverflow.com/q/4789837
                os.killpg(os.getpgid(scheduler.pid), signal.SIGTERM)

    def process_result(self, take_finished_jobs_from_story=False):
        finished_jobs = None
        if take_finished_jobs_from_story:
            finished_jobs = self.__finished_jobs_in_story()
        raw_history = self.__state.gather_history(finished_jobs)
        self.__write_history_to_files(raw_history)

        history = History(raw_history, self.__story.get_raw('jobs'), self.__machines_events, self.__fair_sets_events)
        self.result = ObjectiveFunction(history).compute()

        self.__determine_quasi_optimal_result()

    def __prepare_machines_events(self):
        for m_id, events in self.__story.get_raw('machine_events').items():
            self.__prepare_machine_events(m_id, events)

    def __prepare_machine_events(self, m_id, events):
        prev_dead = None
        for time, new_state in events:
            new_dead = (new_state == int(MachineState.MACHINE_DEAD))
            if new_dead != prev_dead:
                # In raw state there is an artificial machine state == 3
                new_state = MachineState.MACHINE_DEAD if new_dead else MachineState.MACHINE_IDLE
                self.__machines_events.append((m_id, time, new_state))

    def __prepare_fair_sets_events(self):
        for fs_id, events in self.__story.get_raw('fair_service_machine_set_events').items():
            for time, changes in events:
                self.__fair_sets_events.append((fs_id, time, changes[0], changes[1]))

    def __prepare_for_running(self):
        utils.make_empty_dir(self.__lss_assignments_dir)
        utils.make_empty_dir(self.__lss_input_dir)
        self.has_failed = False
        self.state = self.__fresh_state()
        timer.setup(self.__story.get_raw('mint'))

    def __fresh_state(self):
        return State(
            self.__story,
            self.__lss_input_path,
            self.__lss_assignments_dir
        )

    def __determine_quasi_optimal_result(self):
        mint = self.__story.get_raw('mint')
        maxt = self.__story.get_raw('maxt')
        dummy_machine_id = list(self.__story.get_raw('machine_events').keys())[0]
        min_setup_time = min(self.__story.get_raw('context_changes').values())
        finished_jobs = []
        for job in self.__story.get_raw('jobs'):
            if mint <= job['ready'] <= maxt:
                job['real_start_time'] = job['ready']
                job['real_duration'] = min_setup_time + job['expected_duration_barring_setup']
                job['real_machine'] = dummy_machine_id
                finished_jobs.append(job)
        raw_history = self.__state.gather_history(finished_jobs)
        history = History(raw_history, self.__story.get_raw('jobs'), self.__machines_events, self.__fair_sets_events)
        self.quasi_optimal_result = ObjectiveFunction(history, imbalance_factor=0).compute()

    def __finished_jobs_in_story(self):
        mint = self.__story.get_raw('mint')
        maxt = self.__story.get_raw('maxt')
        return [
            job
            for job in self.__story.get_raw('jobs')
            if mint <= job['ready'] and
                job['real_duration'] and
                job['real_duration'] + job['real_start_time'] <= maxt
        ]

    def __write_history_to_files(self, raw_history):
        log_file = os.path.join(self.__log_dir, 'history')
        with open(log_file, 'wb') as f:
            pickle.dump(raw_history, f)
        with open(log_file + '.json', 'wt') as f:
            json.dump(raw_history, f, indent=2)

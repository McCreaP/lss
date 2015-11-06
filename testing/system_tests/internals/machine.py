import os
import shutil
import logging
from enum import IntEnum

LSS_ASSIGNMENTS_DIR = 'assignments'

LOGGER = logging.getLogger('test_runner')


class MachineState(IntEnum):
    MACHINE_IDLE = 0
    MACHINE_WORKING = 1


class Machine:

    def __init__(self, machine_id, lss_input_dir, context_changes_costs):
        self.__id = machine_id
        self.__state = MachineState.MACHINE_IDLE
        self.__lss_assignment_file = os.path.join(
            self.__lss_assignments_path, str(self.__id))
        self.__taken_path = self.__lss_assignment_path + '-taken'
        self.__context = (-1, -1, -1)
        self.__context_changes_costs = context_changes_costs

    def show(self):
        return '%s %s' % (self.__id, int(self.__state))

    def get_state(self):
        return self.__state

    def try_to_take_job(self, now, ready_jobs):
        assert self.__state == MachineState.MACHINE_IDLE
        try:
            job = self.__get_assigned_job(ready_jobs)
        except FileNotFoundError:
            finish_event_args = []
            return finish_event_args
        self.__state = MachineState.MACHINE_WORKING
        job['real_start_time'] = now
        job['real_machine'] = self.__id
        duration = self.__calculate_setup_time(job) + job['expected_duration_barring_setup']
        finish_event_args = [(now + duration, job)]
        self.__context = (job['context1'], job['context2'], job['context3'])
        LOGGER.debug(
            'Job %s has been taken by machine: %s', job_id, self.__id)
        return finish_event_args

    def free(self):
        self.__state = MachineState.MACHINE_IDLE

    def __get_assigned_job(self, ready_jobs):
        os.rename(self.__lss_assignment_path, self.__taken_path)
        with open(self.__taken_path, 'rt') as f:
            job_id = int(f.read())
        assert job_id in ready_jobs
        return ready_jobs[job_id]

    def __calculate_setup_time(self, job):
        return self.__context_changes_costs[(
            self.__context[0] == job['context1'],
            self.__context[1] == job['context2'],
            self.__context[2] == job['context3'])]
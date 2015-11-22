import os
import pickle
import json
import logging

from internals import utils
from internals import timer
from internals.input_writer import InputWriter
from internals.machine import Machine, MachineState
from internals.exceptions import InvalidJobException

LSS_ASSIGNMENTS_DIR = 'assignments'

LOGGER = logging.getLogger('test_runner')


class State:

    def __init__(self, story, lss_input_dir):
        lss_assignments_dir = os.path.join(lss_input_dir, LSS_ASSIGNMENTS_DIR)
        utils.clean_dir(lss_assignments_dir)

        self.__story = story
        self.__input_writer = InputWriter(lss_input_dir, story)

        self.__machines = {
            m['id']: Machine(m['id'], lss_assignments_dir, story.get_raw('context_changes'))
            for m in story.get_raw('machines')
        }

        self.__ready_jobs = {}
        self.__finished_jobs = {}

    def add_ready_job(self, job):
        self.__ready_jobs[job['id']] = job
        self.__input_writer.write(self.__machines, self.__ready_jobs)

    def use_idle_machines(self):
        finish_job_events_args = [
            m.try_to_take_job(timer.now(), self.__ready_jobs)
            for m in self.__machines.values()
            if m.get_state() == MachineState.MACHINE_IDLE
        ]
        return utils.flatten(finish_job_events_args)

    def finish_job(self, job):
        now = timer.now()
        if job not in self.__ready_jobs.values():
            raise InvalidJobException(
                job['id'],
                "Cannot finish job. It probably has already been finished by other machine"
            )
        del self.__ready_jobs[job['id']]
        job['real_duration'] = now - job['real_start_time']
        self.__finished_jobs[job['id']] = job

        machine_id = job['real_machine']
        machine = self.__machines[machine_id]
        assert machine.get_state() == MachineState.MACHINE_WORKING
        machine.free()

    def write_history(self, log_dir):
        log_file = os.path.join(log_dir, 'history')
        history = self.__prepare_history_to_write()
        with open(log_file, 'wb') as f:
            pickle.dump(history, f)
        with open(log_file + '.json', 'wt') as f:
            json.dump(history, f, indent=2)

    def __prepare_history_to_write(self):

        def for_one(k, v):
            if k == 'jobs':
                return list(self.__finished_jobs.values())
            elif k == 'context_changes':
                return list(v.items())
            else:
                return v

        story = {k: for_one(k, v) for k, v in self.__story.get_items()}
        return story

#     def try_to_take_job(self, machine_id):
# return self.__machines[machine_id].try_to_take_job(timer.now(),
# self.__ready_jobs)

import os
import pickle
import json
import logging
from collections import defaultdict

from internals import utils
from internals import timer
from internals.input_writer import InputWriter
from internals.machine import Machine, MachineState
from internals.exceptions import InvalidJobException

LOGGER = logging.getLogger('test_runner')


class State:

    def __init__(self, story, lss_input_path, lss_assignments_dir):
        self.__story = story
        self.__input_writer = InputWriter(lss_input_path, story)
        self.__lss_assignment_dir = lss_assignments_dir
        self.__machines = {}
        self.__machine_sets = defaultdict(set)
        self.__fair_sets = defaultdict(set)
        self.__ready_jobs = {}
        self.__finished_jobs = {}

    def add_ready_job(self, job):
        self.__ready_jobs[job['id']] = job

    def use_idle_machines(self):
        finish_job_events_args = [
            m.try_to_take_job(timer.now(), self.__ready_jobs)
            for m in self.__machines.values()
            if m.get_state() == MachineState.MACHINE_IDLE
        ]
        return utils.flatten(finish_job_events_args)

    def finish_job(self, execution_time, job):
        now = timer.now()
        if job not in self.__ready_jobs.values():
            raise InvalidJobException(
                job['id'],
                "Cannot finish job. It probably has already been finished by other machine"
            )
        machine_id = job['real_machine']
        machine = self.__machines[machine_id]
        finish_event_args = (execution_time, job['id'])
        if finish_event_args in machine.discarded_finish_events:
            machine.discarded_finish_events.remove(finish_event_args)
            return
        del self.__ready_jobs[job['id']]
        job['real_duration'] = now - job['real_start_time']
        self.__finished_jobs[job['id']] = job

        assert machine.get_state() == MachineState.MACHINE_WORKING, \
            "Machine: " + str(machine_id)+ " has state: " + str(machine.get_state())
        assert machine.finish_event_args == finish_event_args, \
            "Unexpected finish event args. " \
            "Expected: " + str(finish_event_args) + \
            ", got: " + str(machine.finish_event_args)
        machine.free()

    def gather_history(self, finished_jobs=None):

        def for_one(k, v):
            if k == 'jobs':
                return finished_jobs if finished_jobs else list(self.__finished_jobs.values())
            elif k == 'context_changes':
                return list(v.items())
            else:
                return v

        story = {k: for_one(k, v) for k, v in self.__story.get_items()}
        return story

    def machine_event_idle(self, machine_id):
        if machine_id in self.__machines.keys():
            self.__machines[machine_id].bring_to_life()
        else:
            self.__machines[machine_id] = Machine(
                machine_id,
                self.__lss_assignment_dir,
                self.__story.get_raw('context_changes')
            )

    def machine_event_dead(self, machine_id):
        if machine_id in self.__machines.keys():
            self.__machines[machine_id].kill()
        else:
            self.__machines[machine_id] = Machine(
                machine_id,
                self.__lss_assignment_dir,
                self.__story.get_raw('context_changes')
            )
            self.__machines[machine_id].kill()

    def add_machines_to_machine_set(self, ms_id, machines):
        self.__machine_sets[ms_id] |= set(machines)

    def remove_machines_from_machine_set(self, ms_id, machines):
        self.__machine_sets[ms_id] -= set(machines)

    def add_machines_to_fair_set(self, ms_id, machines):
        self.__fair_sets[ms_id] |= set(machines)

    def remove_machines_from_fair_set(self, ms_id, machines):
        self.__fair_sets[ms_id] -= set(machines)

    def update_input(self):
        self.__input_writer.write(
            self.__machines,
            self.__ready_jobs.values(),
            self.__machine_sets,
            self.__fair_sets
        )

#     def try_to_take_job(self, machine_id):
# return self.__machines[machine_id].try_to_take_job(timer.now(),
# self.__ready_jobs)

import logging

from internals import utils
from internals import history
from internals import timer
from internals.input_writer import InputWriter
from internals.machine import Machine, MachineState

LSS_ASSIGNMENTS_DIR = 'assignments'

LOGGER = logging.getLogger('test_runner')


class State:

    def __init__(self, story, lss_input_dir):
        self.__story = story
        self.__input_writer = InputWriter(lss_input_dir, story)
        self.__machines = {
            m['id']: Machine(m['id'], lss_input_dir) for m in story['machines']}
        self.__ready_jobs = {}

    def add_ready_job(self, job):
        self.__ready_jobs[job['id']] = job
        self.__input_writer.write(self.__machines, self.__ready_jobs)

    def use_idle_machines(self):
        finish_job_events_args = [m.try_to_take_job(timer.now(), self.__ready_jobs)
                                  for m in self.__machines.values() if m.get_state() == MachineState.MACHINE_IDLE]
        return utils.flatten(finish_job_events_args)

    def finish_job(self, job):
        now = timer.now()
        assert job in self.__ready_jobs.values()
        del self.__ready_jobs[job['id']]
        job['real_duration'] = now - job['real_start_time']
        history.record_job(job)

        machine_id = job['real_machine']
        machine = self.__machines[machine_id]
        assert machine.get_state() == MachineState.MACHINE_WORKING
        machine.free()

#     def try_to_take_job(self, machine_id):
# return self.__machines[machine_id].try_to_take_job(timer.now(),
# self.__ready_jobs)

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

    def __init__(self, machine_id, lss_input_dir):
        self.__id = machine_id
        self.__state = MachineState.MACHINE_IDLE
        self.__lss_assignments_path = os.path.join(
            lss_input_dir, LSS_ASSIGNMENTS_DIR)
        if os.path.exists(self.__lss_assignments_path):
            shutil.rmtree(self.__lss_assignments_path)
            LOGGER.debug("Removed %s", self.__lss_assignments_path)

    def show(self):
        return '%s %s' % (self.__id, int(self.__state))

    def get_state(self):
        return self.__state

    def try_to_take_job(self, now, ready_jobs):
        path = os.path.join(self.__lss_assignments_path, str(self.__id))
        tmp_path = path + '-taken'
        try:
            os.rename(path, tmp_path)
            with open(tmp_path, 'rt') as f:
                job_id = int(f.read())
            assert job_id in ready_jobs
            job = ready_jobs[job_id]
            duration = job['real_duration']  # TODO
    #         setup_time = job['real_setup_time']  # TODO
            new_state = MachineState.MACHINE_WORKING
            job['real_start_time'] = now
    #         job['real_setup_time'] = setup_time
            job['real_machine'] = self.__id
            finish_event_args = [(now + duration, job)]
            LOGGER.debug(
                'Job %s has been taken by machine: %s', job_id, self.__id)
        except FileNotFoundError:
            new_state = MachineState.MACHINE_IDLE
            finish_event_args = []
        self.__state = new_state
        return finish_event_args

    def free(self):
        self.__state = MachineState.MACHINE_IDLE

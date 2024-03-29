import abc
import logging
from internals.machine import MachineState

LOGGER = logging.getLogger('test_runner')

USE_IDLE_MACHINES_INTERVAL = 3
UPDATE_INPUT_INTERVAL = 30


class Event:
    def __init__(self, execution_time):
        self._execution_time = execution_time

    def __str__(self):
        return "Event"

    def __lt__(self, other):
        return self._execution_time < other._execution_time

    def execute(self):
        LOGGER.debug("Event: '%s'", str(self))
        self._execute_impl()

    def get_execution_time(self):
        return self._execution_time

    @abc.abstractmethod
    def _execute_impl(self):
        return


class FinishJob(Event):
    def __init__(self, execution_time, job, state):
        super(FinishJob, self).__init__(execution_time)
        self.__job = job
        # self.__event_loop = event_loop
        self.__state = state

        # self.__machine_id = self.__job['real-machine']

    def __str__(self):
        return "Finish job %s" % self.__job['id']

    def _execute_impl(self):
        self.__state.finish_job(self._execution_time, self.__job)


# finish_event = self.__state.try_to_take_job(self.__machine_id)
#         self.__event_loop.add_event(finish_event)


class UseIdleMachines(Event):
    def __init__(self, execution_time, event_loop, state):
        super(UseIdleMachines, self).__init__(execution_time)
        self.__event_loop = event_loop
        self.__state = state

    def __str__(self):
        return "Looking for jobs assigned to idle machines"

    def _execute_impl(self):
        finish_jobs_args = self.__state.use_idle_machines()
        for finish_job_execution_time, job in finish_jobs_args:
            self.__event_loop.add_event(FinishJob(finish_job_execution_time, job, self.__state))
        self.__event_loop.add_event(UseIdleMachines(
            self._execution_time + USE_IDLE_MACHINES_INTERVAL, self.__event_loop, self.__state))


class JobReady(Event):
    def __init__(self, job, state):
        super(JobReady, self).__init__(job['ready'])
        self.__job = job
        self.__state = state

    def __str__(self):
        return "Job %s ready" % self.__job['id']

    def _execute_impl(self):
        self.__state.add_ready_job(self.__job)


class MachineEvent(Event):
    def __init__(self, m_id, execution_time, machine_state, state):
        super(MachineEvent, self).__init__(execution_time)
        self.__machine_id = m_id
        self.__new_machine_state = machine_state
        self.__state = state

    def __str__(self):
        return "Machine %s changes state to %s" % (self.__machine_id, self.__new_machine_state)

    def _execute_impl(self):
        if self.__new_machine_state == MachineState.MACHINE_IDLE:
            self.__state.machine_event_idle(self.__machine_id)
        elif self.__new_machine_state == MachineState.MACHINE_DEAD:
            self.__state.machine_event_dead(self.__machine_id)


class MachineSetEvent(Event):
    def __init__(self, ms_id, execution_time, changes, state):
        super(MachineSetEvent, self).__init__(execution_time)
        self.__machine_set_id = ms_id
        self.__new_machines = changes[0]
        self.__old_machines = changes[1]
        self.__state = state

    def __str__(self):
        return "Machine set: %s, add machines: %s, remove machines: %s" % \
               (self.__machine_set_id, self.__new_machines, self.__old_machines)

    def _execute_impl(self):
        self.__state.add_machines_to_machine_set(self.__machine_set_id, self.__new_machines)
        self.__state.remove_machines_from_machine_set(self.__machine_set_id, self.__old_machines)


class FairSetEvent(Event):
    def __init__(self, fs_id, execution_time, new_machines, old_machines, state):
        super(FairSetEvent, self).__init__(execution_time)
        self.__machine_set_id = fs_id
        self.__new_machines = new_machines
        self.__old_machines = old_machines
        self.__state = state

    def __str__(self):
        return "%s, add machines: %s, remove machines: %s" % \
               (self.__machine_set_id, self.__new_machines, self.__old_machines)

    def _execute_impl(self):
        self.__state.add_machines_to_fair_set(self.__machine_set_id, self.__new_machines)
        self.__state.remove_machines_from_fair_set(self.__machine_set_id, self.__old_machines)


class UpdateInput(Event):
    def __init__(self, execution_time, event_loop, state):
        super(UpdateInput, self).__init__(execution_time)
        self.__event_loop = event_loop
        self.__state = state

    def __str__(self):
        return "Update input"

    def _execute_impl(self):
        self.__state.update_input()
        self.__event_loop.add_event(UpdateInput(
            self._execution_time + UPDATE_INPUT_INTERVAL,
            self.__event_loop,
            self.__state
        ))

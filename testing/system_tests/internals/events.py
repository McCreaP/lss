import abc
import logging

LOGGER = logging.getLogger('test_runner')

USE_IDLE_MACHINES_INTERVAL = 3


class Event:

    def __init__(self, execution_time):
        self.execution_time = execution_time

    def __str__(self):
        return "Event"

    def __lt__(self, other):
        return self.execution_time < other.execution_time

    def execute(self):
        LOGGER.debug("Event: '%s'", str(self))
        self.execute_impl()

    def get_execution_time(self):
        return self.execution_time

    @abc.abstractmethod
    def execute_impl(self):
        return

class FinishJob(Event):

    def __init__(self, execution_time, job, state):
        super(FinishJob, self).__init__(execution_time)
        self.__job = job
#         self.__event_loop = event_loop
        self.__state = state
#         self.__machine_id = self.__job['real-machine']

    def __str__(self):
        return "Finish job %s" % self.__job['id']

    def execute_impl(self):
        self.__state.finish_job(self.__job)
#         finish_event = self.__state.try_to_take_job(self.__machine_id)
#         self.__event_loop.add_event(finish_event)


class UseIdleMachines(Event):

    def __init__(self, execution_time, event_loop, state):
        super(UseIdleMachines, self).__init__(execution_time)
        self.__event_loop = event_loop
        self.__state = state

    def __str__(self):
        return "Use idle machines"

    def execute_impl(self):
        finish_jobs_args = self.__state.use_idle_machines()
        for finish_job_execution_time, job in finish_jobs_args:
            self.__event_loop.add_event(FinishJob(finish_job_execution_time, job, self.__state))
        self.__event_loop.add_event(UseIdleMachines(
            self.execution_time + USE_IDLE_MACHINES_INTERVAL, self.__event_loop, self.__state))


class JobReady(Event):
    counter = 0

    def __init__(self, job, state):
        super(JobReady, self).__init__(job['ready'])
        self.__job = job
        self.__state = state
        JobReady.counter += 1

    def __str__(self):
        return "Job %s ready" % self.__job['id']

    def execute_impl(self):
        self.__state.add_ready_job(self.__job)
        JobReady.counter -= 1

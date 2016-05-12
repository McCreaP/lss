from queue import PriorityQueue
import logging

from internals.objective_function.events import MachineEvent, StartJob, FinishJob, FairSetEvent

LOGGER = logging.getLogger('test_runner')


class ObjectiveFunction:

    def __init__(self, history, imbalance_factor=1):
        self.__history = history
        self.__events = PriorityQueue()
        self.__imbalance_factor = imbalance_factor

    def compute(self):
        self.__add_events()
        current_value = 0.0
        plot = []
        while not self.__events.empty():
            event = self.__events.get()
            current_value += event.execute()
            plot.append((event.get_execution_time(), current_value))
            LOGGER.debug("Value of objective function at %s: %s",
                         event.get_execution_time(), current_value)
        return current_value, plot

    def __add_events(self):
        self.__add_job_events()
        self.__add_machine_events()
        self.__add_fair_service_events()
        self.__add_dummy_events()

    def __add_job_events(self):
        for job in self.__history.get_jobs():
            self.__events.put(StartJob(job, self.__imbalance_factor))
            self.__events.put(FinishJob(job, self.__imbalance_factor))

    def __add_machine_events(self):
        for m_id, time, new_state in self.__history.machine_events:
            self.__events.put(MachineEvent(m_id, time, new_state))

    def __add_fair_service_events(self):
        for fs_id, time, new_machines, old_machines in self.__history.fair_sets_events:
            self.__events.put(FairSetEvent(fs_id, time, new_machines, old_machines))

    # Just for computing an imbalance ingredient at the end of the interval
    def __add_dummy_events(self):
        for dummy_job in self.__history.get_dummy_jobs():
            self.__events.put(StartJob(dummy_job, self.__imbalance_factor))

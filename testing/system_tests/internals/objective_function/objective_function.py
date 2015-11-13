from queue import PriorityQueue
import logging

from internals.objective_function.events import StartJob, FinishJob

LOGGER = logging.getLogger('test_runner')


class ObjectiveFunction:

    def __init__(self, history):
        self.__history = history
        self.__events = PriorityQueue()

    def compute(self):
        self.__add_events()
        current_value = 0.0
        while not self.__events.empty():
            event = self.__events.get()
            current_value += event.execute()
            LOGGER.debug("Value of objective function at %s: %s",
                         event.get_execution_time(), current_value)
        return current_value

    def __add_events(self):
        for job in self.__history.get_jobs():
            self.__events.put(StartJob(job))
            self.__events.put(FinishJob(job))

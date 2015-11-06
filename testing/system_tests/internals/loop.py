import time
import logging
from queue import PriorityQueue

from internals.events import JobReady
from internals.events import UseIdleMachines
from internals.state import State
from internals import timer


INPUT_UPDATE_PERIOD = 3

LOGGER = logging.getLogger("test_runner")


class EventLoop:

    def __init__(self, story, state):
        self.__story = story
        self.__events = PriorityQueue()
        self.__state = state
        self.__progress_bar = ProgressBar(
            self.__story['mint'], self.__story['maxt'])

    def run(self):
        self.__add_jobs_ready_events()
        self.add_event(UseIdleMachines(timer.now(), self, self.__state))
        # This condition isn't a proper condition yet
        while JobReady.counter > 0:
            event = self.__events.get()
            now = timer.now()
            if now < event.get_execution_time():
                LOGGER.debug("Will sleep for %s s", str(event.get_execution_time() - now))
                time.sleep(event.get_execution_time() - now)
            event.execute()
            self.__progress_bar.show_progress(now)

    def add_event(self, event):
        self.__events.put(event)

    def __add_jobs_ready_events(self):
        for job in self.__story['jobs']:
            self.add_event(JobReady(job, self.__state))


class ProgressBar:

    def __init__(self, min_t, max_t):
        self.__min_t = min_t
        self.__max_t = max_t
        self.__timespan = self.__max_t - self.__min_t
        self.__progress_shown = None

    def show_progress(self, now):
        progress = int(100 * (now - self.__min_t) / (self.__timespan))
        if self.__progress_shown != progress:
            self.__progress_shown = progress
            LOGGER.debug(
                '%s%% of the simulated period elapsed (of %s s)', progress, self.__timespan)

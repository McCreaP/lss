import logging
from queue import PriorityQueue
from internals import timer
from internals.events import JobReady, MachineEvent, MachineSetEvent, FairSetEvent, UseIdleMachines, UpdateInput
from internals.machine import MachineState
from internals.skipper_api import SkipperApi
from threading import Condition

LOGGER = logging.getLogger("test_runner")


class EventLoop:

    def __init__(self, story, state):
        self.__story = story
        self.__events = PriorityQueue()
        self.__state = state
        self.__progress_bar = ProgressBar(self.__story.get_raw('mint'), self.__story.get_raw('maxt'))
        self.__condition = Condition()
        self.__api_skipper = SkipperApi(self.stop_waiting)

    def run(self):
        self.__add_jobs_ready_events()
        self.__add_machines_events()
        self.__add_machine_sets_events()
        self.__add_fair_sets_events()
        self.add_event(UseIdleMachines(timer.now(), self, self.__state))
        self.add_event(UpdateInput(timer.now(), self, self.__state))
        while timer.now() < self.__story.get_raw('maxt'):
            event = self.__events.get()
            now = timer.now()
            if now < event.get_execution_time():
                if type(event) is UpdateInput:
                    time_left = event.get_execution_time() - now
                    self.__wait_for(time_left)
                timer.setup(event.get_execution_time())
            event.execute()
            self.__progress_bar.show_progress(now)

    def stop_waiting(self):
        self.__condition.acquire()
        self.__condition.notify()
        self.__condition.release()

    def add_event(self, event):
        self.__events.put(event)

    def __add_jobs_ready_events(self):
        for job in self.__story.get_raw('jobs'):
            self.add_event(JobReady(job, self.__state))

    def __add_machines_events(self):
        for m_id, events in self.__story.get_raw('machine_events').items():
            self.__add_machine_events(m_id, events)

    def __add_machine_events(self, m_id, events):
        prev_dead = None
        for time, new_state in events:
            new_dead = (new_state == int(MachineState.MACHINE_DEAD))
            if new_dead != prev_dead:
                # In raw state there is an artificial machine state == 3
                new_state = MachineState.MACHINE_DEAD if new_dead else MachineState.MACHINE_IDLE
                self.add_event(MachineEvent(m_id, time, new_state, self.__state))

    def __add_machine_sets_events(self):
        for ms_id, events in self.__story.get_raw('machine_set_events').items():
            for time, changes in events:
                self.add_event(MachineSetEvent(ms_id, time, changes, self.__state))

    def __add_fair_sets_events(self):
        for fs_id, events in self.__story.get_raw('fair_service_machine_set_events').items():
            for time, changes in events:
                self.add_event(FairSetEvent(fs_id, time, changes, self.__state))

    def __wait_for(self, time):
        LOGGER.debug("Will wait for scheduler for %s s", str(time))
        self.__condition.acquire()
        self.__condition.wait(time)
        self.__condition.release()


class ProgressBar:

    def __init__(self, min_t, max_t):
        self.__min_t = min_t
        self.__max_t = max_t
        self.__timespan = self.__max_t - self.__min_t
        self.__progress_shown = None

    def show_progress(self, now):
        progress = int(100 * (now - self.__min_t) / self.__timespan)
        if self.__progress_shown != progress:
            self.__progress_shown = progress
            LOGGER.debug('%s%% of the simulated period elapsed (of %s s)',
                         progress, self.__timespan)

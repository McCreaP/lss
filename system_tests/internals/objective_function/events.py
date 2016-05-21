import abc
import logging

from internals.machine import MachineState

LOGGER = logging.getLogger('test_runner')


class Event:
    def __init__(self, execution_time):
        self._execution_time = execution_time

    def __lt__(self, other):
        return self._execution_time < other._execution_time

    def execute(self):
        LOGGER.debug("Compute objective function event: %s", str(self))
        value = self._execute_impl()
        LOGGER.debug("Add %d to the objective function", value)
        return value

    def get_execution_time(self):
        return self._execution_time

    @abc.abstractmethod
    def _execute_impl(self):
        return


class StartJob(Event):
    def __init__(self, job, imbalance_factor):
        super(StartJob, self).__init__(job.get_start_time())
        self._job = job
        self._imbalance_factor = imbalance_factor

    def __str__(self):
        return "Start: %s at %s" % (str(self._job), self._execution_time)

    def _execute_impl(self):
        return -self._imbalance_factor * self.__imbalance_ingredient()

    def __imbalance_ingredient(self):
        fair_machine_set = self._job.get_fair_machine_set()
        if not fair_machine_set:
            return 0
        return fair_machine_set.start_job(self._job.get_account_id(), self._execution_time)


class FinishJob(Event):
    def __init__(self, job, imbalance_factor):
        super(FinishJob, self).__init__(job.get_finish_time())
        self._job = job
        self._imbalance_factor = imbalance_factor

    def __str__(self):
        return "Finish: %s at %s" % (str(self._job), self._execution_time)

    def _execute_impl(self):
        value = -self._imbalance_factor * self.__imbalance_ingredient()
        value += self.__finish_job_ingredient()
        return value

    def __imbalance_ingredient(self):
        fair_machine_set = self._job.get_fair_machine_set()
        if not fair_machine_set:
            return 0
        imbalance_ingredient = fair_machine_set.finish_job(self._job.get_account_id(), self._execution_time)
        LOGGER.debug("Imbalance ingredient: %f", imbalance_ingredient)
        return imbalance_ingredient

    def __finish_job_ingredient(self):
        batch = self._job.get_batch()
        finish_job_ingredient = batch.finish_job(self._job)
        LOGGER.debug("Finish job ingredient: %s", finish_job_ingredient)
        return finish_job_ingredient


class MachineEvent(Event):
    def __init__(self, machine, time, new_state):
        super(MachineEvent, self).__init__(time)
        self.__machine = machine
        self.__new_state = new_state

    def __str__(self):
        return "Machine event, machine id: %s, new state: %s, at %s" % \
               (str(self.__machine.id), str(self.__new_state), self._execution_time)

    def _execute_impl(self):
        if self.__new_state == MachineState.MACHINE_DEAD:
            return self.__machine.dead(self._execution_time)
        else:
            return self.__machine.idle(self._execution_time)


class FairSetEvent(Event):
    def __init__(self, fair_set, time, new_machines, old_machines):
        super(FairSetEvent, self).__init__(time)
        self.__fair_set = fair_set
        self.__new_machines = new_machines
        self.__old_machines = old_machines

    def __str__(self):
        return "Fair machine set event: %s at %s" % (str(self.__fair_set), self._execution_time)

    def _execute_impl(self):
        value = 0.
        for machine in self.__new_machines:
            value += self.__fair_set.add_machine(machine, self._execution_time)
            machine.fair_machine_set = self.__fair_set
        for machine in self.__old_machines:
            value += self.__fair_set.remove_machine(machine, self._execution_time)
            machine.fair_machine_set = None
        return value

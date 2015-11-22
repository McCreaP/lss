import abc
import logging

LOGGER = logging.getLogger('test_runner')


class JobEvent:

    def __init__(self, execution_time, job, imbalance_factor):
        self._execution_time = execution_time
        self._job = job
        self._imbalance_factor = imbalance_factor

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


class StartJob(JobEvent):

    def __init__(self, job, imbalance_factor):
        super(StartJob, self).__init__(job.get_start_time(), job, imbalance_factor)

    def __str__(self):
        return "Start: %s at %s" % (str(self._job), self._execution_time)

    def _execute_impl(self):
        return -self._imbalance_factor * self.__imbalance_ingredient()

    def __imbalance_ingredient(self):
        fair_machine_set = self._job.get_fair_machine_set()
        return fair_machine_set.start_job(self._job.get_account_id(), self._execution_time)


class FinishJob(JobEvent):

    def __init__(self, job, imbalance_factor):
        super(FinishJob, self).__init__(job.get_finish_time(), job, imbalance_factor)

    def __str__(self):
        return "Finish: %s at %s" % (str(self._job), self._execution_time)

    def _execute_impl(self):
        value = -self._imbalance_factor * self.__imbalance_ingredient()
        value += self.__finish_job_ingredient()
        return value

    def __imbalance_ingredient(self):
        fair_machine_set = self._job.get_fair_machine_set()
        imbalance_ingredient = fair_machine_set.finish_job(self._job.get_account_id(), self._execution_time)
        LOGGER.debug("Imbalance_ingredient", imbalance_ingredient)
        return imbalance_ingredient

    def __finish_job_ingredient(self):
        batch = self._job.get_batch()
        finish_job_ingredient = batch.finish_job(self._job)
        LOGGER.debug("Finish job ingredient", finish_job_ingredient)
        return finish_job_ingredient

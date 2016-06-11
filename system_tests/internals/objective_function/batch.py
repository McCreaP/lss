import math


class Batch:

    def __init__(self, raw_batch, number_of_jobs):
        self.__raw_batch = raw_batch
        self.__number_of_jobs = number_of_jobs
        self.__number_of_finished_jobs = 0

    def get_raw(self, key):
        return self.__raw_batch[key]

    def finish_job(self, job):
        self.__number_of_finished_jobs += 1
        r = (job.get_finish_time() - self.__raw_batch['due']) / self.__raw_batch['T']
        penalty_for_job_tardiness = self.__compute_penalty_for_job_tardiness(r)
        penalty_for_batch_tardiness = self.__compute_penalty_for_batch_tardiness(r) \
            if self.__all_jobs_finished() \
            else 0.0
        return penalty_for_job_tardiness + penalty_for_batch_tardiness

    def __compute_penalty_for_job_tardiness(self, r):
        return self.__sigmoid(r, self.__raw_batch['A'], self.__raw_batch['B'])

    def __compute_penalty_for_batch_tardiness(self, r):
        return self.__sigmoid(r, self.__raw_batch['C'], self.__raw_batch['D'])

    @staticmethod
    def __sigmoid(r, reward, timely_reward):
        return timely_reward / (1 + math.exp(r)) + reward

    def __all_jobs_finished(self):
        return self.__number_of_finished_jobs == self.__number_of_jobs

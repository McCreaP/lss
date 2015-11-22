import math


class Batch:

    def __init__(self, raw_batch, number_of_jobs):
        self.__raw_batch = raw_batch
        self.__number_of_jobs = number_of_jobs
        self.__number_of_finished_jobs = 0
        # According to the paper it should be nonzero only for the largest batches
        # but so far we don't have batch's 'beta' value
        self.__beta = 1 / self.__number_of_jobs if self.__number_of_jobs else 0

    def get_raw(self, key):
        return self.__raw_batch[key]
    
    def finish_job(self, job):
        self.__number_of_finished_jobs += 1
        sigmoid = self.__sigmoid((job.get_finish_time() - self.__raw_batch['due']) / self.__raw_batch['T'])
        penalty_for_job_tardiness = self.__beta * sigmoid
        penalty_for_batch_tardiness = sigmoid if self.__all_jobs_finished() else 0.0
        return penalty_for_job_tardiness + penalty_for_batch_tardiness

    def __sigmoid(self, x):
        A = self.__raw_batch['A']
        B = self.__raw_batch['B']
        return A / (1 + math.exp(x)) + B

    def __all_jobs_finished(self):
        return self.__number_of_finished_jobs == self.__number_of_jobs

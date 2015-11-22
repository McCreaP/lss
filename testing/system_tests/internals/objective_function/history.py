from collections import defaultdict

from .job import Job
from .batch import Batch
from .fair_machine_set import FairMachineSet
from .job import DummyJob


class History:

    def __init__(self, raw_history, all_raw_jobs):
        self.__raw_history = raw_history

        number_of_jobs_in_batch = defaultdict(int)
        for raw_job in all_raw_jobs:
            number_of_jobs_in_batch[raw_job['batch']] += 1
        self.__batches = {
            raw_batch['id']: Batch(raw_batch, number_of_jobs_in_batch[raw_batch['id']])
            for raw_batch in self.__raw_history['batches']
        }

        raw_accounts = self.__raw_history['accounts']
        mint = self.__raw_history['mint']
        maxt = self.__raw_history['maxt']
        raw_fair_machine_sets = self.__raw_history['fair_service_machine_sets']
        fair_machine_sets = [
            FairMachineSet(raw_fair_machine_set, raw_accounts, mint)
            for raw_fair_machine_set in raw_fair_machine_sets
        ]
        fair_machine_set_of_machine = {}
        for fair_machine_set in fair_machine_sets:
            for machine_id in fair_machine_set.get_machines_ids():
                fair_machine_set_of_machine[machine_id] = fair_machine_set               
        self.__jobs = [
            Job(raw_job, self.__batches[raw_job['batch']], fair_machine_set_of_machine[raw_job['real_machine']])
            for raw_job in self.__raw_history['jobs']
        ]

        self.__dummy_jobs = []
        for fair_machine_set in fair_machine_sets:
            for raw_account in raw_accounts:
                self.__dummy_jobs.append(DummyJob(raw_account['id'], fair_machine_set, maxt))

    def get_raw(self, key):
        return self.__raw_history[key]

    def get_jobs(self):
        return self.__jobs

    def get_dummy_jobs(self):
        return self.__dummy_jobs
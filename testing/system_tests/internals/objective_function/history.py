from internals.objective_function.job import Job
from internals.objective_function.batch import Batch
from internals.objective_function.fair_machine_set import FairMachineSet
from collections import defaultdict


class History:

    def __init__(self, raw_history, raw_jobs):
        self.__raw_history = raw_history

        number_of_jobs = defaultdict(int)
        for raw_job in raw_jobs:
            number_of_jobs[raw_job['batch']] += 1
        self.__batches = {
            raw_batch['id']: Batch(raw_batch, number_of_jobs[raw_batch['id']])
            for raw_batch in self.__raw_history['batches']
        }

        raw_accounts = self.__raw_history['accounts']
        mint = self.__raw_history['mint']
        fair_service_machine_sets = self.__raw_history['fair_service_machine_sets']
        fair_machine_sets = {
            fair_machine_set['id']: FairMachineSet(fair_machine_set, raw_accounts, mint)
            for fair_machine_set in fair_service_machine_sets
        }
        fair_machine_set_of_machine = {}
        for fair_machine_set in fair_machine_sets.values():
            for machine_id in fair_machine_set.get_machines_ids():
                fair_machine_set_of_machine[machine_id] = fair_machine_set               
        self.__jobs = {
            raw_job['id']: Job(raw_job, self.__batches[raw_job['batch']], fair_machine_set_of_machine[raw_job['real_machine']])
            for raw_job in self.__raw_history['jobs']
        }

    def get_raw(self, key):
        return self.__raw_history[key]

    def get_jobs(self):
        return self.__jobs.values()

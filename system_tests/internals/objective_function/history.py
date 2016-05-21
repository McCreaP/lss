from collections import defaultdict

from internals.objective_function.machine import Machine
from .job import Job
from .batch import Batch
from .fair_machine_set import FairMachineSet
from .job import DummyJob


class History:

    def __init__(self, raw_history, all_raw_jobs, machine_events, fair_sets_events):
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

        fair_machine_sets = {
            fs_id: FairMachineSet(fs_id, raw_accounts, mint)
            for fs_id in self.__raw_history['fair_service_machine_set_events']
        }

        machines = {
            m_id: Machine(m_id)
            for m_id, _ in self.__raw_history['machine_events'].items()
        }

        self.machine_events = [
            (machines[m_id], time, new_state)
            for m_id, time, new_state in machine_events
        ]

        self.fair_sets_events = [(
                fair_machine_sets[fs_id],
                time,
                [machines[m_id] for m_id in new_machines_ids],
                [machines[m_id] for m_id in old_machines_ids]
            )
            for fs_id, time, new_machines_ids, old_machines_ids in fair_sets_events
        ]

        self.__jobs = [
            Job(raw_job, self.__batches[raw_job['batch']], machines[raw_job['real_machine']])
            for raw_job in self.__raw_history['jobs']
        ]

        self.__dummy_jobs = []
        for fs_id in fair_machine_sets.keys():
            for raw_account in raw_accounts:
                self.__dummy_jobs.append(DummyJob(raw_account['id'], fair_machine_sets[fs_id], maxt))

    def get_raw(self, key):
        return self.__raw_history[key]

    def get_jobs(self):
        return self.__jobs

    def get_dummy_jobs(self):
        return self.__dummy_jobs

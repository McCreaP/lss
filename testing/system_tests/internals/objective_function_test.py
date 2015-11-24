#!/usr/bin/env python3
import unittest
from objective_function.objective_function import ObjectiveFunction
from objective_function.history import History


SAMPLE_HISTORY = {
    'fair_service_machine_sets': [
        {
            'id': 1,
            'initial_contents': [1, 2, 3]
        },
        {
            'id': 2,
            'initial_contents': [4]
        },
        {
            'id': 3,
            'initial_contents': [5, 6]
        }
    ],
    'jobs': [
    ],
    'batches': [
        {
            'id': 1,
            'account': 1,
            'A': 1.0,
            'B': 1.0,
            'T': 1.0,
            'due': 14.0
        },
        {
            'id': 2,
            'account': 2,
            'A': 3.5,
            'B': 1.5,
            'T': 5.0,
            'due': 16.0
        },
        {
            'id': 3,
            'account': 3,
            'A': 2.0,
            'B': 7.0,
            'T': 4.5,
            'due': 23.5
        },
        {
            'id': 4,
            'account': 2,
            'A': 5.5,
            'B': 3.5,
            'T': 8.5,
            'due': 25.0
        }
    ],
    'accounts': [
        {
            'id': 1,
            'alloc': 0.2
        },
        {
            'id': 2,
            'alloc': 0.7
        },
        {
            'id': 3,
            'alloc': 0.1
        }
    ],
    'mint': 0.0,
    'maxt': 30.0
}


class RawHistoryBuilder:

    def __init__(self):
        self.__raw_history = SAMPLE_HISTORY

    def with_finished_jobs(self, raw_jobs):
        for raw_job in raw_jobs:
            self.__raw_history['jobs'].append(raw_job)
        return self

    def get(self):
        return self.__raw_history


class RawJobBuilder:

    def __init__(self):
        self.__raw_job = {
            'id': 0,
            'batch': 0,
            'real_start_time': 0.0,
            'real_duration': 0.0,
            'real_machine': 0
        }

    def with_id(self, job_id):
        self.__raw_job['id'] = job_id
        return self

    def with_batch(self, batch_id):
        self.__raw_job['batch'] = batch_id
        return self

    def with_start_time(self, time):
        self.__raw_job['real_start_time'] = time
        return self

    def with_duration(self, duration):
        self.__raw_job['real_duration'] = duration
        return self

    def with_machine(self, machine_id):
        self.__raw_job['real_machine'] = machine_id
        return self

    def get(self):
        return self.__raw_job


JOB_1 = RawJobBuilder().with_id(1) \
    .with_batch(1) \
    .with_start_time(1) \
    .with_duration(5) \
    .with_machine(1) \
    .get()


class TestObjectiveFunction(unittest.TestCase):
    PRECISION = 5

    def test_no_jobs(self):
        raw_history = RawHistoryBuilder().get()
        value = ObjectiveFunction(History(raw_history, [])).compute()
        self.assertEqual(-90.0, value)

    def test_one_job(self):
        all_raw_jobs = [JOB_1]
        raw_history = RawHistoryBuilder().with_finished_jobs(all_raw_jobs).get()
        actual_value = ObjectiveFunction(History(raw_history, all_raw_jobs)).compute()
        expected_value = -89.0 + 3.999329299739
        self.assertEqual(round(expected_value, self.PRECISION), round(actual_value, self.PRECISION))


if __name__ == '__main__':
    unittest.main()

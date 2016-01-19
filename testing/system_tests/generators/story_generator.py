from random import randrange
import itertools
from time import time
from .objects import Base, Machine, FairServiceMachineSet, MachineSet, Job, Batch, \
    ContextChange, Account


class StoryGenerator(Base):
    def __init__(self, mint: float = None, maxt: float = None, duration: float = None, machine_count: int = None,
                 machine_set_count: int = None,
                 fair_service_machine_set_count: int = None, job_count: int = None, batch_count: int = None,
                 account_count: int = None):
        assert maxt or duration
        self.mint = mint or time()
        self.maxt = maxt or self.mint + duration
        self.generate_machines(machine_count)
        self.generate_jobs(job_count)
        self.generate_machine_sets(machine_set_count)
        self.generate_fair_service_machine_sets(fair_service_machine_set_count)
        self.generate_batches(batch_count)
        self.generate_accounts(account_count)
        self.generate_context_changes()
        self.machine_events = []
        self.names_of_machine_sets = {}
        self.names_of_machines = {}
        self.names_of_batches = {}
        self.names_of_accounts = {}

    def generate_machines(self, count):
        self.machines = [Machine(id) for id in range(count)]

    def generate_machine_sets(self, count):
        assert self.jobs
        self.machine_sets = self._generate_machine_sets(count, MachineSet)
        for j in self.jobs:
            j.machine_set = self.machine_sets[randrange(count)]

    def generate_fair_service_machine_sets(self, count):
        self.fair_service_machine_sets = self._generate_machine_sets(count, FairServiceMachineSet)

    def _generate_machine_sets(self, count, cls):
        machine_sets = [cls(id) for id in range(count)]
        for m in self.machines:
            machine_sets[randrange(count)] += m
        return machine_sets

    def generate_jobs(self, count):
        self.jobs = [Job(id, self.mint, self.maxt) for id in range(count)]

    def generate_batches(self, count):
        assert self.jobs
        self.batches = [Batch(id, self.mint, self.maxt) for id in range(count)]
        for j in self.jobs:
            self.batches[randrange(count)] += j

    def generate_accounts(self, count):
        assert self.jobs
        self.accounts = [Account(id) for id in range(count)]
        for j in self.jobs:
            self.accounts[randrange(count)] += j
        for b in self.batches:
            b.account = self.accounts[randrange(count)]

    def generate_context_changes(self):
        self.context_changes = [ContextChange(changes) for changes in itertools.product('01', repeat=3)]

    def generate_machine_events(self, count):
        if count:
            raise NotImplementedError()

    def dump(self):
        result = {}
        for key in self.__dict__:
            if key is "context_changes":
                result[key] = {k: v for cc in self.context_changes for k, v in cc.dump().items()}
            elif isinstance(getattr(self, key), list):
                result[key] = [o.dump() for o in getattr(self, key)]
            else:
                result[key] = getattr(self, key)
        return result

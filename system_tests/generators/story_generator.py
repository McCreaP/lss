import itertools
import random
from time import time
from random import randrange
from .objects import Base, Machine, FairServiceMachineSet, MachineSet, Job, Batch, \
    ContextChange, Account


class StoryGenerator(Base):
    """
    Generates small system tests

    """

    def __init__(self, mint: float = None, maxt: float = None, duration: float = None,
                 machine_count: int = None, machine_set_count: int = None, fair_service_machine_set_count: int = None,
                 job_count: int = None, batch_count: int = None, account_count: int = None, seed: int = None):
        if seed:
            random.seed(seed)
        assert (maxt is None) ^ (duration is None)
        self.mint = mint or time()
        self.maxt = maxt or self.mint + duration
        self._generate_machines(machine_count)
        self._generate_jobs(job_count)
        self._generate_machine_sets(machine_set_count)
        self._generate_fair_service_machine_sets(fair_service_machine_set_count)
        self._generate_batches(batch_count)
        self._generate_accounts(account_count)
        self._generate_context_changes()
        self.machine_events = []
        self.names_of_machine_sets = {}
        self.names_of_machines = {}
        self.names_of_batches = {}
        self.names_of_accounts = {}

    def _generate_machines(self, count):
        self.machines = [Machine(id) for id in range(count)]

    def _generate_machine_sets(self, count):
        assert self.jobs
        self.machine_sets = self._generate_machine_sets_base(count, MachineSet)
        for j in self.jobs:
            j.machine_set = self.machine_sets[randrange(count)]

    def _generate_fair_service_machine_sets(self, count):
        self.fair_service_machine_sets = self._generate_machine_sets_base(count, FairServiceMachineSet)

    def _generate_machine_sets_base(self, count, cls):
        assert self.machines
        machine_sets = [cls(id) for id in range(count)]
        for m in self.machines:
            machine_sets[randrange(count)] += m
        return machine_sets

    def _generate_jobs(self, count):
        self.jobs = [Job(id, self.mint, self.maxt) for id in range(count)]

    def _generate_batches(self, count):
        assert self.jobs
        self.batches = [Batch(id, self.mint, self.maxt) for id in range(count)]
        for j in self.jobs:
            self.batches[randrange(count)] += j

    def _generate_accounts(self, count):
        assert self.jobs
        self.accounts = [Account(id) for id in range(count)]
        for b in self.batches:
            b.account = self.accounts[randrange(count)]

    def _generate_context_changes(self):
        self.context_changes = [ContextChange(changes) for changes in itertools.product('01', repeat=3)]

    def _generate_machine_events(self, count):
        if count:
            raise NotImplementedError()

    def dump(self):
        """
        Returns:
            Test represented in native python data structures ready for pickle serialization
        """
        result = {}
        for key in self.__dict__:
            if key is "context_changes":
                result[key] = {k: v for cc in self.context_changes for k, v in cc.dump().items()}
            elif isinstance(getattr(self, key), list):
                result[key] = [o.dump() for o in getattr(self, key)]
            else:
                result[key] = getattr(self, key)
        return result

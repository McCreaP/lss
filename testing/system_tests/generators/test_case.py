from random import randrange
import itertools
from .objects import Base, Machine, FairServiceMachineSet, MachineSet, Job, Batch, \
    ContextChange, Account


class TestCase(Base):
    def __init__(self):
        self.machines = []
        self.machine_sets = []
        self.fair_service_machine_sets = []
        self.jobs = []
        self.batches = []
        self.accounts = []
        self.context_changes = []

    @staticmethod
    def generate(machines: int, jobs: int, machine_sets: int, fair_service_machine_sets: int, batches: int,
                 accounts: int):
        tc = TestCase()
        tc.generate_machines(machines)
        tc.generate_jobs(jobs)
        tc.generate_machine_sets(machine_sets)
        tc.generate_fair_service_machine_sets(fair_service_machine_sets)
        tc.generate_batches(batches)
        tc.generate_accounts(accounts)
        tc.generate_context_changes()
        return tc

    def generate_machines(self, count):
        self.machines = [Machine.rand(id) for id in range(count)]

    def generate_machine_sets(self, count):
        assert self.jobs
        self.machine_sets = self._generate_machine_sets(count, MachineSet)
        for j in self.jobs:
            j.machine_set = self.machine_sets[randrange(count)]

    def generate_fair_service_machine_sets(self, count):
        self.fair_service_machine_sets = self._generate_machine_sets(count, FairServiceMachineSet)

    def _generate_machine_sets(self, count, cls):
        assert self.machines
        machine_sets = [cls.rand(id) for id in range(count)]
        for m in self.machines:
            machine_sets[randrange(count)] += m
        return machine_sets

    def generate_jobs(self, count):
        self.jobs = [Job.rand(id) for id in range(count)]

    def generate_batches(self, count):
        assert self.jobs
        self.batches = [Batch.rand(id) for id in range(count)]
        for j in self.jobs:
            self.batches[randrange(count)] += j

    def generate_accounts(self, count):
        assert self.jobs
        self.accounts = [Account.rand(id) for id in range(count)]
        for j in self.jobs:
            self.accounts[randrange(count)] += j

    def generate_context_changes(self):
        self.context_changes = [ContextChange.rand(changes) for changes in itertools.product('01', repeat=3)]

    def dumps(self):
        return ("\n" * 2).join(["machines\n" + "\n".join(repr(m) for m in self.machines),
                                "machine-sets\n" + "\n".join(repr(m) for m in self.machine_sets),
                                "fair-service-machine-sets\n" +
                                "\n".join(repr(f) for f in self.fair_service_machine_sets),
                                "jobs\n" + "\n".join(repr(j) for j in self.jobs),
                                "batches\n" + "\n".join(repr(b) for b in self.batches),
                                "accounts\n" + "\n".join(repr(a) for a in self.accounts),
                                "context-changes\n" + "\n".join(repr(c) for c in self.context_changes)])

    def dump(self, fp):
        fp.write(self.dumps())

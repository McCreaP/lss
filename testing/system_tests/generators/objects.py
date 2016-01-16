from enum import IntEnum
from random import uniform, randrange
from sys import maxsize
from typing import Iterable, List, Tuple


def rand(upper_bound=maxsize):
    return randrange(upper_bound)


def randf(upper_bound):
    return uniform(0, upper_bound)


class MachineState(IntEnum):
    MACHINE_IDLE = 0
    MACHINE_WORKING = 1
    MACHINE_DEAD = 2


class Base:
    def __str__(self, *args, **kwargs):
        return "{0.__class__.__name__}: {0!r}".format(self)


class Machine(Base):
    def __init__(self, machine_id: int, state: MachineState = MachineState.MACHINE_IDLE):
        self.id = machine_id
        self.state = state

    def __repr__(self, *args, **kwargs):
        return "{0.id!s} {0.state.value!s}".format(self)

    @staticmethod
    def rand(id=None):
        id = rand() if id is None else id
        return Machine(id, MachineState(randrange(3)))


class BaseMachineSet(Base):
    def __init__(self, machine_set_id: int, machines: List[Machine]):
        self.id = machine_set_id
        self.machines = machines

    def __iadd__(self, machine: Machine):
        self.machines.append(machine)
        return self

    def __repr__(self, *args, **kwargs):
        return "{0.id!s} {1}".format(self, " ".join([str(m.id) for m in self.machines]))


class MachineSet(BaseMachineSet):
    @staticmethod
    def rand(id=None):
        id = rand() if id is None else id
        return MachineSet(id, [])


class FairServiceMachineSet(BaseMachineSet):
    @staticmethod
    def rand(id=None):
        id = rand() if id is None else id
        return FairServiceMachineSet(id, [])


class Context(Base):
    def __init__(self, context: Tuple[int, int, int]):
        self.context = context

    def __repr__(self, *args, **kwargs):
        return "{0.context[0]!s} {0.context[1]!s} {0.context[2]!s}".format(self)

    @staticmethod
    def rand(upper_bound=5):
        return Context((rand(upper_bound), rand(upper_bound), rand(upper_bound)))


class Job(Base):
    # TODO MachineSet or FairServiceMachineSet?
    def __init__(self, job_id: int, expected_duration_barring_setup: float, context: Context):
        self.id = job_id
        self.batch = None
        self.account = None
        self.expected_duration_barring_setup = expected_duration_barring_setup
        self.machine_set = None
        self.context = context

    def __repr__(self, *args, **kwargs):
        return "{0.id!s} {0.batch.id!s} {0.account.id!s} {0.expected_duration_barring_setup:.3f} {0.machine_set.id!s} {1!r}" \
            .format(self, self.context)

    @staticmethod
    def rand(id=None):
        id = rand() if id is None else id
        return Job(id, randf(100), Context.rand())


class Batch(Base):
    def __init__(self, batch_id: int, A: float, B: float, T: float, due: float):
        self.id = batch_id
        self.A = A
        self.B = B
        self.T = T
        self.due = due

    def __repr__(self, *args, **kwargs):
        return "{0.id!s} {0.A:.3f} {0.B:.3f} {0.T:.3f} {0.due:.3f}".format(self)

    def __iadd__(self, job: Job):
        job.batch = self
        return self

    @staticmethod
    def rand(id=None):
        id = rand() if id is None else id
        return Batch(id, randf(100), randf(100), randf(100), randf(100))


class Account(Base):
    def __init__(self, account_id: int, alloc: float):
        self.id = account_id
        self.alloc = alloc

    def __repr__(self, *args, **kwargs):
        return "{0.id!s} {0.alloc:.3f}".format(self)

    def __iadd__(self, job: Job):
        job.account = self
        return self

    @staticmethod
    def rand(id=None):
        id = rand() if id is None else id
        return Account(id, randf(1))


class ContextChange(Base):
    def __init__(self, changes: Tuple[int, int, int], switch: int):
        self.changes = changes
        self.switch = switch

    def __repr__(self, *args, **kwargs) -> str:
        return "{0} {1.switch!s}".format(" ".join([str(c) for c in self.changes]), self)

    @staticmethod
    def rand(changes):
        return ContextChange(changes, rand(1000))

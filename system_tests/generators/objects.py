from enum import IntEnum
from random import uniform, randrange
from sys import maxsize
from typing import Iterable, List, Tuple, Union
from copy import deepcopy


def rand(upper_bound=maxsize):
    return round(randrange(upper_bound), 2)


def randf(x: float, y: float = 0):
    return round(uniform(x, y), 2)


class MachineState(IntEnum):
    MACHINE_IDLE = 0
    MACHINE_WORKING = 1
    MACHINE_DEAD = 2


class Base:
    pass


class Machine(Base):
    def __init__(self, id: int, state: MachineState = MachineState.MACHINE_IDLE):
        self.id = id
        self.state = state

    def dump(self):
        return {"id": self.id}


class Job(Base):
    def __init__(self, id: int, mint: float, maxt: float):
        self.id = id
        self.batch = None
        self.ready = randf(mint, maxt)
        self.expected_duration_barring_setup = randf(maxt - self.ready)
        self.machine_set = None
        self.context = Context(upper_bound=5)

    def dump(self):
        result = deepcopy(self.__dict__)
        result["batch"] = self.batch.id
        result["machine_set"] = self.machine_set.id
        del result["context"]
        result["context1"], result["context2"], result["context3"] = self.context.dump()
        return result


class BaseMachineSet(Base):
    def __init__(self, machine_set_id: int, machines: List[Machine] = None):# pylint: disable=E1126
        self.id = machine_set_id
        self.machines = machines or []

    def __iadd__(self, machine: Machine):
        self.machines.append(machine)
        return self

    def dump(self):
        return {
            'id': self.id,
            'initial_contents': [m.id for m in self.machines]
        }


class MachineSet(BaseMachineSet):
    def __iadd__(self, other: Union[Job, Machine]):
        if isinstance(other, Job):
            other.machine_set = self
            return self
        else:
            return super(MachineSet, self).__iadd__(other)


class FairServiceMachineSet(BaseMachineSet):
    pass


class Context(Base):
    def __init__(self, context: Tuple[int, int, int] = None, upper_bound: int = None):
        assert context or upper_bound
        self.context = context or (rand(upper_bound), rand(upper_bound), rand(upper_bound))

    def dump(self):
        return self.context


class Batch(Base):
    def __init__(self, id: int, mint: float, maxt: float):
        self.id = id
        self.account = None
        self.A = randf(100)
        self.B = randf(100)
        self.T = randf(100)
        self.due = randf(mint, maxt)

    def __iadd__(self, job: Job):
        job.batch = self
        return self

    def dump(self):
        result = deepcopy(self.__dict__)
        result["account"] = self.account.id
        return result


class Account(Base):
    def __init__(self, id: int, alloc: float = None):
        self.id = id
        self.alloc = alloc or randf(1)

    def dump(self):
        return deepcopy(self.__dict__)


class ContextChange(Base):
    def __init__(self, changes: Tuple[int, int, int], switch: int = None):
        self.changes = changes
        self.switch = switch or rand(1000)

    def dump(self):
        return {self.changes: self.switch}

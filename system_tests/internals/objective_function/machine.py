from internals.machine import MachineState


class Machine:
    def __init__(self, m_id):
        self.id = m_id
        self.fair_machine_set = None
        self.state = None

    def dead(self, execution_time):
        self.state = MachineState.MACHINE_DEAD
        return self.fair_machine_set.machine_dead(self.id, execution_time) if self.fair_machine_set else 0

    def idle(self, execution_time):
        self.state = MachineState.MACHINE_IDLE
        return self.fair_machine_set.machine_active(self.id, execution_time) if self.fair_machine_set else 0

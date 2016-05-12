from internals.machine import MachineState


class FairMachineSet:

    def __init__(self, fs_id, raw_accounts, mint):
        self.__id = fs_id
        self.__dead_machines = set()
        self.__active_machines = set()
        self.__accounts = {raw_acc['id']: raw_acc for raw_acc in raw_accounts}
        self.__active_jobs_ct = {acc_id: 0 for acc_id in self.__accounts.keys()}
        self.__last_change = {acc_id: mint for acc_id in self.__accounts.keys()}

    def __str__(self):
        return "Fair machine set: %d" % self.__id

    def start_job(self, account_id, execution_time):
        value = self.__compute_from_last_update(account_id, execution_time)
        self.__active_jobs_ct[account_id] += 1
        return value

    def finish_job(self, account_id, execution_time):
        value = self.__compute_from_last_update(account_id, execution_time)
        self.__active_jobs_ct[account_id] -= 1
        return value

    def add_machine(self, machine, execution_time):
        value = self.__compute_for_all_accounts(execution_time)
        if machine.state == MachineState.MACHINE_DEAD:
            self.__dead_machines.add(machine.id)
        else:
            self.__active_machines.add(machine.id)
        return value

    def remove_machine(self, machine, execution_time):
        value = self.__compute_for_all_accounts(execution_time)
        if machine.state == MachineState.MACHINE_DEAD:
            self.__dead_machines.remove(machine.id)
        else:
            self.__active_machines.remove(machine.id)
        return value

    def machine_active(self, machine_id, execution_time):
        value = self.__compute_for_all_accounts(execution_time)
        if machine_id in self.__dead_machines:
            self.__dead_machines.remove(machine_id)
        self.__active_machines.add(machine_id)
        return value

    def machine_dead(self, machine_id, execution_time):
        value = self.__compute_for_all_accounts(execution_time)
        if machine_id in self.__active_machines:
            self.__active_machines.remove(machine_id)
        self.__dead_machines.add(machine_id)
        return value

    def __compute_for_all_accounts(self, execution_time):
        value = 0.
        for account_id in self.__accounts.keys():
            value += self.__compute_from_last_update(account_id, execution_time)
        return value

    def __compute_from_last_update(self, account_id, execution_time):
        period_from_last_change = execution_time - self.__last_change[account_id]
        fraction = self.__accounts[account_id]['alloc']
        if self.__active_machines:
            fraction = self.__active_jobs_ct[account_id] / len(self.__active_machines)
        imbalance = max(0, self.__accounts[account_id]['alloc'] - fraction)
        self.__last_change[account_id] = execution_time
        return period_from_last_change * imbalance

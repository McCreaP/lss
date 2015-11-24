class FairMachineSet:

    def __init__(self, raw_fair_machine_set, raw_accounts, mint):
        self.__raw_fair_machine_set = raw_fair_machine_set
        self.__number_of_available_machines = len(raw_fair_machine_set['initial_contents'])
        self.__accounts = {raw_acc['id']: raw_acc for raw_acc in raw_accounts}
        self.__active_jobs_ct = {acc_id: 0 for acc_id in self.__accounts.keys()}
        self.__last_change = {acc_id: mint for acc_id in self.__accounts.keys()}

    def __str__(self):
        return "Fair machine set: %d" % self.__raw_fair_machine_set['id']

    def get_machines_ids(self):
        return self.__raw_fair_machine_set['initial_contents']

    def start_job(self, account_id, execution_time):
        value = self.__compute_from_last_update(account_id, execution_time)
        self.__active_jobs_ct[account_id] += 1
        self.__last_change[account_id] = execution_time
        return value

    def finish_job(self, account_id, execution_time):
        value = self.__compute_from_last_update(account_id, execution_time)
        self.__active_jobs_ct[account_id] -= 1
        self.__last_change[account_id] = execution_time
        return value

    def __compute_from_last_update(self, account_id, execution_time):
        period_from_last_change = execution_time - self.__last_change[account_id]
        imbalance = max(0, self.__accounts[account_id]['alloc'] - self.__active_jobs_ct[account_id] / self.__number_of_available_machines)
        return period_from_last_change * imbalance

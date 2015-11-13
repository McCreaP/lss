class Job:

    def __init__(self, raw_job, batch, fair_machine_set):
        self.__raw_job = raw_job
        self.__batch = batch
        self.__fair_machine_set = fair_machine_set

    def __str__(self):
        return "Job %s" % str(self.__raw_job['id'])

    def get_account_id(self):
        return self.__batch.get_raw('account')

    def get_fair_machine_set(self):
        return self.__fair_machine_set

    def get_batch(self):
        return self.__batch

    def get_start_time(self):
        return self.__raw_job['real_start_time']

    def get_finish_time(self):
        return self.__raw_job['real_start_time'] + self.__raw_job['real_duration']
    
    def get_id(self):
        return self.__raw_job['id']

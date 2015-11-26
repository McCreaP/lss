import os

from internals import utils


class InputWriter:
    MACHINES_FILE = 'machines'
    MACHINE_SETS_FILE = 'machine-set'
    FAIR_MACHINE_SETS_FILE = 'fair-service-machine-sets'
    JOBS_FILE = 'jobs'
    BATCHES_FILE = 'batches'
    ACCOUNTS_FILE = 'accounts'
    CONTEXT_CHANGES_FILE = 'context-changes'

    def __init__(self, lss_input_dir, story):
        self.__lss_input_dir = lss_input_dir
        self.__story = story
        self.__clean_input_dir()

    def write(self, machines, ready_jobs):
        self.__write_to_files([
            (self.MACHINES_FILE, show_machines(machines)),
            (self.MACHINE_SETS_FILE, show_machine_sets(self.__story.get_raw('machine_sets'))),
            (self.FAIR_MACHINE_SETS_FILE, show_machine_sets(self.__story.get_raw('fair_service_machine_sets'))),
            (self.JOBS_FILE, show_jobs(ready_jobs)),
            (self.BATCHES_FILE, show_batches(self.__story.get_raw('batches'))),
            (self.ACCOUNTS_FILE, show_accounts(self.__story.get_raw('accounts'))),
            (self.CONTEXT_CHANGES_FILE, show_context_changes(self.__story.get_raw('context_changes')))
        ])

    def __clean_input_dir(self):
        self.__rm_files_from_input_dir([
            self.MACHINES_FILE,
            self.MACHINE_SETS_FILE,
            self.FAIR_MACHINE_SETS_FILE,
            self.JOBS_FILE,
            self.BATCHES_FILE,
            self.ACCOUNTS_FILE,
            self.CONTEXT_CHANGES_FILE
        ])

    def __rm_files_from_input_dir(self, file_names):
        for file_name in file_names:
            utils.remove_file(os.path.join(self.__lss_input_dir, file_name))

    def __write_to_files(self, data):
        for file_name, lines in data:
            self.__write_to_file(file_name, lines)

    def __write_to_file(self, file_name, lines):
        file_path = os.path.join(self.__lss_input_dir, file_name)
        tmp_path = file_path + '-new'
        with open(tmp_path, 'wt') as f:
            for line in lines:
                f.write(line + '\n')
        os.rename(tmp_path, file_path)


def show_fields(fields):
    return ' '.join([str(field) for field in fields])


def show_machines(machines):
    return [m.show() for m in machines.values()]


def show_machine_sets(raw_machine_sets):
    return [show_machine_set(raw_machine_set) for raw_machine_set in raw_machine_sets]


def show_machine_set(machine_set):
    return '%s %s' % (machine_set['id'], ' '.join([str(machine_id) for machine_id in machine_set['initial_contents']]))


def show_jobs(raw_jobs):
    return [show_job(raw_job) for raw_job in raw_jobs]


def show_job(job):
    return show_fields([
        job['id'],
        job['batch'],
        job['expected_duration_barring_setup'],
        job['machine_set'],
        job['context1'],
        job['context2'],
        job['context3']
    ])


def show_batches(raw_batches):
    return [show_batch(raw_batch) for raw_batch in raw_batches]


def show_batch(batch):
    return show_fields([
        batch['id'],
        batch['account'],
        batch['A'],
        batch['B'],
        batch['T'],
        batch['due']
    ])


def show_accounts(raw_accounts):
    return [show_account(raw_account) for raw_account in raw_accounts]


def show_account(account):
    return show_fields([
        account['id'],
        account['alloc']
    ])


def show_context_changes(x):
    def one(difference, expected_setup_time):
        return show_fields(difference + (expected_setup_time, ))

    return [one(*args) for args in x.items()]

import os

from internals import utils


class InputWriter:
    INPUT_FILE = 'input'

    def __init__(self, lss_input_dir, story):
        self.__lss_input_path = os.path.join(lss_input_dir, InputWriter.INPUT_FILE)
        self.__story = story
        self.__clean_input_dir()

    def write(self, machines, ready_jobs):
        pieces = [
            ('machines', show_machines(machines)),
            ('machine-sets', show_machine_sets(self.__story.get_raw('machine_sets'))),
            ('fair-service-machine-sets', show_machine_sets(self.__story.get_raw('fair_service_machine_sets'))),
            ('jobs', show_jobs(ready_jobs)),
            ('batches', show_batches(self.__story.get_raw('batches'))),
            ('accounts', show_accounts(self.__story.get_raw('accounts'))),
            ('context-changes', show_context_changes(self.__story.get_raw('context_changes')))
        ]

        def glue(title, data):
            return [title] + data + ['']

        self.__write_input_file(utils.flatten(glue(*p) for p in pieces))

    def __clean_input_dir(self):
        utils.remove_file(self.__lss_input_path)

    def __write_input_file(self, lines):
        tmp_path = self.__lss_input_path + '-new'
        with open(tmp_path, 'wt') as f:
            for line in lines:
                f.write(line + '\n')
        os.rename(tmp_path, self.__lss_input_path)


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

import os

from internals import utils


class InputWriter:

    def __init__(self, lss_input_file, story):
        self.__lss_input_file = lss_input_file
        self.__story = story

    def write(self, machines, ready_jobs):
        pieces = \
            [('machines', [m.show() for m in machines.values()]),
             ('machine-sets', [show_machine_set(x)
                               for x in self.__story.get_raw('machine_sets')]),
             ('fair-service-machine-sets', [show_machine_set(x)
                                            for x in self.__story.get_raw('fair_service_machine_sets')]),
             ('jobs', [show_job(job) for job in ready_jobs.values()]),
             ('batches', [show_batch(x) for x in self.__story.get_raw('batches')]),
             ('accounts', [show_account(x) for x in self.__story.get_raw('accounts')]),
             ('context-changes', show_context_changes(self.__story.get_raw('context_changes')))
             ]

        def glue(title, data):
            return [title] + data + ['']

        self.__write_input_file(utils.flatten(glue(*p) for p in pieces))

    def __write_input_file(self, lines):
        tmp_path = self.__lss_input_file + '-new'
        with open(tmp_path, 'wt') as f:
            for line in lines:
                f.write(line + '\n')
        os.rename(tmp_path, self.__lss_input_file)


def show_fields(fields):
    return ' '.join([str(field) for field in fields])


def show_machine_set(machine_set):
    return '{} {}'.format(machine_set['id'], ' '.join([str(machine_id) for machine_id in machine_set['initial_contents']]))


def show_job(j):
    return show_fields(
        [j['id'], j['batch'], j['expected_duration_barring_setup'], j['machine_set'], j['context1'], j['context2'], j['context3']
         ])


def show_batch(b):
    return show_fields(
        [b['id'], b['account'], b['A'], b['B'], b['T'], b['due']
         ])


def show_account(a):
    return show_fields(
        [a['id'], a['alloc']
         ])


def show_context_changes(x):
    def one(difference, expected_setup_time):
        return show_fields(difference + (expected_setup_time, ))

    return [one(*args) for args in x.items()]

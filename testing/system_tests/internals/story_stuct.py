timestamp = float

structure = \
        {'machines':
            [{'id': int
             }
            ]
        ,'machine_sets':
            [{'id': int
             ,'initial_contents': [int]
             }
            ]
        ,'fair_service_machine_sets':
            [{'id': int
             ,'initial_contents': [int]
             }
            ]
        ,'jobs':
            [{'id': int
             ,'batch': int
             ,'expected_duration_barring_setup': float
             ,'machine_set': int
             ,'context1': int
             ,'context2': int
             ,'context3': int
             ,'anticipated': timestamp
             ,'ready': timestamp
             ,'real_start_time': timestamp
             ,'real_setup_time': float
             ,'real_duration': float
             ,'real_machine': int
             }
            ]
        ,'batches':
            [{'id': int
             ,'account': int
             ,'A': float
             ,'B': float
             ,'T': float
             ,'due': timestamp
             }
            ]
        ,'accounts':
             [{'id': int
              ,'alloc': float
              }
             ]
        ,'context_changes': dict  # or a list of pairs; TODO

        ,'mint': timestamp
        ,'maxt': timestamp
        ,'machine_events': [(timestamp, str, int)]

        ,'names_of_machine_sets': dict
        ,'names_of_machines': dict
        ,'names_of_batches': dict
        ,'names_of_accounts': dict
        }
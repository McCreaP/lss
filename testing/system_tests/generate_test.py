#!/usr/bin/env python3
import random

from generators.test_case import TestCase

random.seed(42)
TestCase.generate(machines=10,
                  jobs=30,
                  machine_sets=3,
                  fair_service_machine_sets=2,
                  batches=3,
                  accounts=10).dump(open("temp", "w"))

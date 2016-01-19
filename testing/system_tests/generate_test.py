#!/usr/bin/env python3
import pprint
import random
# from testing.system_tests.generators.story_generator import StoryGenerator
from generators.story_generator import StoryGenerator

random.seed(42)

story = StoryGenerator(duration=1000,
                       machine_count=5,
                       machine_set_count=3,
                       fair_service_machine_set_count=2,
                       job_count=5,
                       batch_count=3,
                       account_count=3)
pp = pprint.PrettyPrinter(indent=1, stream=open("temp", "w"))
pp.pprint(story.dump())

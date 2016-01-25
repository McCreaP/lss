#!/usr/bin/env python3
import pickle
from generators.story_generator import StoryGenerator

if __name__ == '__main__':
    story = StoryGenerator(duration=20,
                           machine_count=5,
                           machine_set_count=3,
                           fair_service_machine_set_count=2,
                           job_count=5,
                           batch_count=3,
                           account_count=3,
                           seed=42)
    pickle.dump(story.dump(), open("./scenarios/generated-story1", "wb"))

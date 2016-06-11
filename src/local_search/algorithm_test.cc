#include "local_search/algorithm.h"

#include <algorithm>
#include <vector>

#include "gtest/gtest.h"

namespace lss {
namespace local_search {
namespace {

auto kSample = RawSituation()
    .time_stamp(0)
    .add(RawMachine().id(0))
    .add(RawMachineSet().id(0).add(0))
    .add(RawAccount().id(0))
    .add(RawBatch().id(0).account(0).timely_reward(1).duration(0.1).due(2))
    .add(RawBatch().id(1).account(0).timely_reward(1).duration(0.1).due(5))
    .add(RawBatch().id(2).account(0).timely_reward(1).duration(0.1).due(3))
    .add(RawBatch().id(3).account(0).timely_reward(1).duration(0.1).due(4))
    .add(RawJob().id(0).batch(0).machine_set(0).duration(1))
    .add(RawJob().id(1).batch(1).machine_set(0).duration(1))
    .add(RawJob().id(2).batch(2).machine_set(0).duration(1))
    .add(RawJob().id(3).batch(3).machine_set(0).duration(1));


// Verify that algorithm starts by assigning all jobs.
TEST(LocalSearchAlgorithmTest, AllAssigned) {
  LocalSearchAlgorithm algorithm(0, 0);
  Situation situation(kSample, false);
  Schedule schedule(situation);
  schedule = algorithm.Run(schedule, situation);

  auto jobs = schedule.GetAssignments().at(situation[Id<Machine>(0)]);
  EXPECT_EQ(situation.jobs().size(), jobs.size());
  for (auto job : situation.jobs()) {
    EXPECT_NE(jobs.end(), std::find(jobs.begin(), jobs.end(), job));
  }
}

// Verify that algorithm finds the optimal schedule given enough iterations
// (for a simple case of single machine).
TEST(LocalSearchAlgorithm, CanImprove) {
  LocalSearchAlgorithm algorithm(20, 0);
  Situation situation(kSample, false);
  Schedule schedule(situation);
  schedule = algorithm.Run(schedule, situation);

  auto machine = situation[Id<Machine>(0)];
  auto job = [&situation](int id) { return situation[Id<Job>(id)]; };
  std::vector<Job> expected{job(0), job(2), job(3), job(1)};
  EXPECT_EQ(expected, schedule.GetAssignments().at(machine));
}

}  // namespace
}  // namespace local_search
}  // namespace lss

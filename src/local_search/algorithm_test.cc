#include "local_search/algorithm.h"

#include <algorithm>

#include "gtest/gtest.h"

namespace lss {
namespace local_search {
namespace {

// Verify that algorithm assigns all jobs if allowed enough iterations.
TEST(DISABLED_LocalSearchAlgorithmTest, AllAssigned) {
  auto raw = RawSituation()
      .add(RawMachine().id(0))
      .add(RawMachineSet().id(0).add(0))
      .add(RawAccount().id(0))
      .add(RawBatch().id(0).account(0))
      .add(RawJob().id(0).batch(0))
      .add(RawJob().id(1).batch(0));
  Situation situation(raw, false);

  Schedule schedule(situation);
  LocalSearchAlgorithm algorithm(2, 0);
  schedule = algorithm.Run(schedule, situation);

  auto jobs = schedule.GetJobsAssignedToMachine(situation[Id<Machine>(0)]);
  EXPECT_EQ(2, jobs.size());
  for (int i = 0; i < 2; ++i) {
    EXPECT_NE(jobs.end(), std::find(jobs.begin(), jobs.end(), situation[Id<Job>(i)]));
  }
}

}  // namespace
}  // namespace local_search
}  // namespace lss

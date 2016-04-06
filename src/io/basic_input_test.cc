#include "io/basic_input.h"

#include <sstream>

#include "gtest/gtest.h"

namespace lss {
namespace io {

// TODO(kamilz): Write unit tests for BasicReader (need to find a way
// to mock filesystem).

TEST(InputOperators, JobInput) {
  static const RawJob kJob = {1, 7, -1, 18, -1, 293, {4, 3, 2}};
  static const std::string data = "1 7 18 293 4 3 2";
  std::istringstream input(data);

  RawJob job;
  input >> job;
  EXPECT_EQ(kJob.id_, job.id_);
  EXPECT_EQ(kJob.batch_, job.batch_);
  EXPECT_EQ(kJob.duration_, job.duration_);
  EXPECT_EQ(kJob.machine_set_, job.machine_set_);
  for (int i = 0; i < Context::kSize; ++i)
    EXPECT_EQ(kJob.context_[i], job.context_[i]);
}

TEST(InputOperators, BatchInput) {
  static const RawBatch kBatch = {5, 2, 50, 234, 4, 34, 10, 44};
  static const std::string data = "5 2 50 234 4 34 10 44";
  std::istringstream input(data);

  RawBatch batch;
  input >> batch;
  EXPECT_EQ(kBatch.id_, batch.id_);
  EXPECT_EQ(kBatch.account_, batch.account_);
  EXPECT_EQ(kBatch.job_reward_, batch.job_reward_);
  EXPECT_EQ(kBatch.job_timely_reward_, batch.job_timely_reward_);
  EXPECT_EQ(kBatch.reward_, batch.reward_);
  EXPECT_EQ(kBatch.timely_reward_, batch.timely_reward_);
  EXPECT_EQ(kBatch.duration_, batch.duration_);
  EXPECT_EQ(kBatch.due_, batch.due_);
}

TEST(InputOperators, MachineInput) {
  static const RawMachine kMachine = {1, MachineState::kWorking, {-1, -1, -1}};
  static const std::string data = "1 1";
  std::istringstream input(data);

  RawMachine machine;
  input >> machine;
  EXPECT_EQ(kMachine.id_, machine.id_);
  EXPECT_EQ(kMachine.state_, machine.state_);
}

TEST(InputOperators, MachineSetInput) {
  static const RawMachineSet kMachineSet = {1, {2, 3, 4, 5, 6}};
  static const std::string data = "1 2 3 4 5 6";
  std::istringstream input(data);

  RawMachineSet machine_set;
  input >> machine_set;
  EXPECT_EQ(kMachineSet.id_, machine_set.id_);
  EXPECT_EQ(kMachineSet.machines_, machine_set.machines_);
}

TEST(InputOperators, FairSetInput) {
  static const RawMachineSet kMachineSet = {1, {2, 3, 4, 5, 6}};
  static const std::string data = "1 2 3 4 5 6";
  std::istringstream input(data);

  RawFairSet fair_set;
  input >> fair_set;
  EXPECT_EQ(kMachineSet.id_, fair_set.id_);
  EXPECT_EQ(kMachineSet.machines_, fair_set.machines_);
}

TEST(InputOperators, AccountInput) {
  static const RawAccount kAccount = {1, 4.55};
  static const std::string data = "1 4.55";
  std::istringstream input(data);

  RawAccount account;
  input >> account;
  EXPECT_EQ(kAccount.id_, account.id_);
  EXPECT_EQ(kAccount.alloc_, account.alloc_);
}

TEST(InputOperators, ContextChangeInput) {
  static const RawChangeCost kContextChange = {{true, false, true}, 20};
  static const std::string data = "1 0 1 20";
  std::istringstream input(data);

  RawChangeCost context_change;
  input >> context_change;
  for (int i = 0; i < Context::kSize; ++i)
    EXPECT_EQ(kContextChange.change_[i], context_change.change_[i]);
  EXPECT_EQ(kContextChange.cost_, context_change.cost_);
}

}  // namespace io
}  // namespace lss

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
  EXPECT_EQ(kJob.id, job.id);
  EXPECT_EQ(kJob.batch_id, job.batch_id);
  EXPECT_EQ(kJob.duration, job.duration);
  EXPECT_EQ(kJob.machineset_id, job.machineset_id);
  for (int i = 0; i < Context::kSize; ++i)
    EXPECT_EQ(kJob.context[i], job.context[i]);
}

TEST(InputOperators, BatchInput) {
  static const RawBatch kBatch = {5, 2, 50, 234, 4, 34, 10, 44};
  static const std::string data = "5 2 50 234 4 34 10 44";
  std::istringstream input(data);

  RawBatch batch;
  input >> batch;
  EXPECT_EQ(kBatch.id, batch.id);
  EXPECT_EQ(kBatch.account_id, batch.account_id);
  EXPECT_EQ(kBatch.job_reward, batch.job_reward);
  EXPECT_EQ(kBatch.job_timely_reward, batch.job_timely_reward);
  EXPECT_EQ(kBatch.reward, batch.reward);
  EXPECT_EQ(kBatch.timely_reward, batch.timely_reward);
  EXPECT_EQ(kBatch.expected_time, batch.expected_time);
  EXPECT_EQ(kBatch.due, batch.due);
}

TEST(InputOperators, MachineInput) {
  static const RawMachine kMachine = {1, MachineState::kWorking, {-1, -1, -1}};
  static const std::string data = "1 1";
  std::istringstream input(data);

  RawMachine machine;
  input >> machine;
  EXPECT_EQ(kMachine.id, machine.id);
  EXPECT_EQ(kMachine.state, machine.state);
}

TEST(InputOperators, MachineSetInput) {
  static const RawMachineSet kMachineSet = {1, {2, 3, 4, 5, 6}};
  static const std::string data = "1 2 3 4 5 6";
  std::istringstream input(data);

  RawMachineSet machine_set;
  input >> machine_set;
  EXPECT_EQ(kMachineSet.id, machine_set.id);
  EXPECT_EQ(kMachineSet.machines, machine_set.machines);
}

TEST(InputOperators, AccountInput) {
  static const RawAccount kAccount = {1, 4.55};
  static const std::string data = "1 4.55";
  std::istringstream input(data);

  RawAccount account;
  input >> account;
  EXPECT_EQ(kAccount.id, account.id);
  EXPECT_EQ(kAccount.alloc, account.alloc);
}

TEST(InputOperators, ContextChangeInput) {
  static const RawContextChange kContextChange = {{true, false, true}, 20};
  static const std::string data = "1 0 1 20";
  std::istringstream input(data);

  RawContextChange context_change;
  input >> context_change;
  for (int i = 0; i < Context::kSize; ++i)
    EXPECT_EQ(kContextChange.changed[i], context_change.changed[i]);
  EXPECT_EQ(kContextChange.cost, context_change.cost);
}

}  // namespace io
}  // namespace lss

#include "io/basic_input.h"

#include <sstream>

#include "gtest/gtest.h"

namespace lss {
namespace io {

// TODO(kamilz): Write unit tests for BasicReader (need to find a way
// to mock filesystem).

TEST(InputOperators, JobInput) {
  static const std::string data = "1 7 18 293 4 3 2";
  std::istringstream input(data);

  RawJob job;
  input >> job;
  EXPECT_EQ(1, job.id_);
  EXPECT_EQ(7, job.batch_);
  EXPECT_EQ(18.0, job.duration_);
  EXPECT_EQ(293, job.machine_set_);
  EXPECT_EQ(Context(4, 3, 2), job.context_);
}

TEST(InputOperators, BatchInput) {
  static const std::string data = "5 2 50 234 4 34 10 44";
  std::istringstream input(data);

  RawBatch batch;
  input >> batch;
  EXPECT_EQ(5, batch.id_);
  EXPECT_EQ(2, batch.account_);
  EXPECT_EQ(50.0, batch.job_reward_);
  EXPECT_EQ(234.0, batch.job_timely_reward_);
  EXPECT_EQ(4.0, batch.reward_);
  EXPECT_EQ(34.0, batch.timely_reward_);
  EXPECT_EQ(10.0, batch.duration_);
  EXPECT_EQ(44.0, batch.due_);
}

TEST(InputOperators, MachineInput) {
  static const std::string data = "2 1";
  std::istringstream input(data);

  RawMachine machine;
  input >> machine;
  EXPECT_EQ(2, machine.id_);
  EXPECT_EQ(MachineState::kWorking, machine.state_);
}

TEST(InputOperators, MachineSetInput) {
  static const std::string data = "1 2 3 4 5 6";
  std::istringstream input(data);

  RawMachineSet machine_set;
  input >> machine_set;
  EXPECT_EQ(1, machine_set.id_);
  EXPECT_EQ(std::vector<IdType>({2, 3, 4, 5, 6}), machine_set.machines_);
}

TEST(InputOperators, FairSetInput) {
  static const std::string data = "1 2 3 4 5 6";
  std::istringstream input(data);

  RawFairSet fair_set;
  input >> fair_set;
  EXPECT_EQ(1, fair_set.id_);
  EXPECT_EQ(std::vector<IdType>({2, 3, 4, 5, 6}), fair_set.machines_);
}

TEST(InputOperators, AccountInput) {
  static const std::string data = "1 4.55";
  std::istringstream input(data);

  RawAccount account;
  input >> account;
  EXPECT_EQ(1, account.id_);
  EXPECT_EQ(4.55, account.alloc_);
}

TEST(InputOperators, ContextChangeInput) {
  static const std::string data = "1 0 1 20";
  std::istringstream input(data);

  RawChangeCost context_change;
  input >> context_change;
  EXPECT_EQ(Change(1, 0, 1), context_change.change_);
  EXPECT_EQ(20, context_change.cost_);
}

}  // namespace io
}  // namespace lss

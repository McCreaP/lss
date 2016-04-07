#include "greedy/machine_wrapper.h"

#include <vector>

#include "gtest/gtest.h"

namespace lss {
namespace greedy {

TEST(MachineWrapper, GetId) {
  static const int kMachineId = 42;
  static const auto kRawMachine = RawMachine().id(kMachineId).state(MachineState::kIdle);

  MachineWrapper machine(kRawMachine, std::make_shared<ContextChanges>());
  EXPECT_EQ(kMachineId, machine.GetId());
}

TEST(MachineWrapper, AssignJob) {
  static const auto kRawMachine = RawMachine().id(1).state(MachineState::kIdle);
  RawJob raw_job_1 = RawJob();
  raw_job_1.id_ = 1;
  raw_job_1.context_[0] = 3; raw_job_1.context_[1] = 5; raw_job_1.context_[2] = 8;
  RawJob raw_job_2 = RawJob();
  raw_job_2.id_ = 2;
  raw_job_2.context_[0] = 3; raw_job_2.context_[1] = 999; raw_job_2.context_[2] = 999;
  static const int kContextChangeCost = 42;
  static const auto kRawContextChange = RawChangeCost()
      .change(Change(0, 1, 1)).cost(kContextChangeCost);
  std::shared_ptr<ContextChanges> context_changes = std::make_shared<ContextChanges>();
  context_changes->SetContextChanges({kRawContextChange});

  MachineWrapper machine(kRawMachine, context_changes);
  EXPECT_TRUE(machine.IsWaitingForAJob());
  machine.AssignJob(raw_job_1);
  EXPECT_FALSE(machine.IsWaitingForAJob());
  EXPECT_EQ(kContextChangeCost, machine.ContextChangeCost(raw_job_2));
}

TEST(MachineWrapper, WorkingMachineIsNotWaitingForAJob) {
  static const auto kRawMachine = RawMachine().id(1).state(MachineState::kWorking);

  MachineWrapper machine(kRawMachine, std::make_shared<ContextChanges>());
  EXPECT_FALSE(machine.IsWaitingForAJob());
}

TEST(MachineWrapper, DeadMachineIsNotWaitingForAJob) {
  static const auto kRawMachine = RawMachine().id(1).state(MachineState::kDead);

  MachineWrapper machine(kRawMachine, std::make_shared<ContextChanges>());
  EXPECT_FALSE(machine.IsWaitingForAJob());
}

TEST(MachineWrapper, SetStateFromWorkingToIdle) {
  static const auto kRawMachine = RawMachine().id(1).state(MachineState::kWorking);

  MachineWrapper machine(kRawMachine, std::make_shared<ContextChanges>());
  EXPECT_FALSE(machine.IsWaitingForAJob());
  machine.SetState(MachineState::kIdle);
  EXPECT_TRUE(machine.IsWaitingForAJob());
}

TEST(MachineWrapper, SetStateFromIdleToIdleWithAssignedJob) {
  static const auto kRawMachine = RawMachine().id(1).state(MachineState::kIdle);
  static const auto kRawJob = RawJob();

  MachineWrapper machine(kRawMachine, std::make_shared<ContextChanges>());
  EXPECT_TRUE(machine.IsWaitingForAJob());
  machine.AssignJob(kRawJob);
  EXPECT_FALSE(machine.IsWaitingForAJob());
  machine.SetState(MachineState::kIdle);
  EXPECT_FALSE(machine.IsWaitingForAJob());
}

}  // namespace greedy
}  // namespace lss

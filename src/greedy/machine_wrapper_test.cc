#include "greedy/machine_wrapper.h"

#include <vector>

#include "gtest/gtest.h"

namespace lss {
namespace greedy {

TEST(MachineWrapper, GetId) {
  static const int kMachineId = 42;
  static const RawMachine kRawMachine = {kMachineId, MachineState::kIdle, {-1, -1, -1}};

  MachineWrapper machine(kRawMachine, std::make_shared<ContextChanges>());
  EXPECT_EQ(kMachineId, machine.GetId());
}

TEST(MachineWrapper, AssignJob) {
  static const RawMachine kRawMachine = {1, MachineState::kIdle, {-1, -1, -1}};
  RawJob raw_job_1 = RawJob();
  raw_job_1.id = 1;
  raw_job_1.context[0] = 3; raw_job_1.context[1] = 5; raw_job_1.context[2] = 8;
  RawJob raw_job_2 = RawJob();
  raw_job_2.id = 2;
  raw_job_2.context[0] = 3; raw_job_2.context[1] = 999; raw_job_2.context[2] = 999;
  static const int kContextChangeCost = 42;
  static const RawContextChange kRawContextChange = {Change(0, 1, 1), kContextChangeCost};
  std::shared_ptr<ContextChanges> context_changes = std::make_shared<ContextChanges>();
  context_changes->SetContextChanges({kRawContextChange});

  MachineWrapper machine(kRawMachine, context_changes);
  EXPECT_TRUE(machine.IsWaitingForAJob());
  machine.AssignJob(raw_job_1);
  EXPECT_FALSE(machine.IsWaitingForAJob());
  EXPECT_EQ(kContextChangeCost, machine.ContextChangeCost(raw_job_2));
}

TEST(MachineWrapper, WorkingMachineIsNotWaitingForAJob) {
  static const RawMachine kRawMachine = {1, MachineState::kWorking, {-1, -1, -1}};

  MachineWrapper machine(kRawMachine, std::make_shared<ContextChanges>());
  EXPECT_FALSE(machine.IsWaitingForAJob());
}

TEST(MachineWrapper, DeadMachineIsNotWaitingForAJob) {
  static const RawMachine kRawMachine = {1, MachineState::kDead, {-1, -1, -1}};

  MachineWrapper machine(kRawMachine, std::make_shared<ContextChanges>());
  EXPECT_FALSE(machine.IsWaitingForAJob());
}

TEST(MachineWrapper, SetStateFromWorkingToIdle) {
  static const RawMachine kRawMachine = {1, MachineState::kWorking, {-1, -1, -1}};

  MachineWrapper machine(kRawMachine, std::make_shared<ContextChanges>());
  EXPECT_FALSE(machine.IsWaitingForAJob());
  machine.SetState(MachineState::kIdle);
  EXPECT_TRUE(machine.IsWaitingForAJob());
}

TEST(MachineWrapper, SetStateFromIdleToIdleWithAssignedJob) {
  static const RawMachine kRawMachine = {1, MachineState::kIdle, {-1, -1, -1}};
  static const RawJob kRawJob = RawJob();

  MachineWrapper machine(kRawMachine, std::make_shared<ContextChanges>());
  EXPECT_TRUE(machine.IsWaitingForAJob());
  machine.AssignJob(kRawJob);
  EXPECT_FALSE(machine.IsWaitingForAJob());
  machine.SetState(MachineState::kIdle);
  EXPECT_FALSE(machine.IsWaitingForAJob());
}

}  // namespace greedy
}  // namespace lss

#include "oo_input_types/machine.h"

#include <vector>

#include "gtest/gtest.h"

namespace lss {

TEST(Machine, GetId) {
  static const int kMachineId = 42;
  static const io::RawMachine kRawMachine = {kMachineId, io::MachineState::kIdle};

  Machine machine(kRawMachine, std::make_shared<ContextChanges>());
  EXPECT_EQ(kMachineId, machine.GetId());
}

TEST(Machine, AssignJob) {
  static const io::RawMachine kRawMachine = {1, io::MachineState::kIdle};
  io::RawJob raw_job_1 = io::RawJob();
  raw_job_1.id = 1;
  raw_job_1.context[0] = 3; raw_job_1.context[1] = 5; raw_job_1.context[2] = 8;
  io::RawJob raw_job_2 = io::RawJob();
  raw_job_2.id = 2;
  raw_job_2.context[0] = 3; raw_job_2.context[1] = 999; raw_job_2.context[2] = 999;
  static const int kContextChangeCost = 42;
  static const io::RawContextChange kRawContextChange = {0, 1, 1, kContextChangeCost};
  std::shared_ptr<ContextChanges> context_changes = std::make_shared<ContextChanges>();
  context_changes->SetContextChanges({kRawContextChange});

  Machine machine(kRawMachine, context_changes);
  EXPECT_TRUE(machine.IsWaitingForAJob());
  machine.AssignJob(raw_job_1);
  EXPECT_FALSE(machine.IsWaitingForAJob());
  EXPECT_EQ(kContextChangeCost, machine.ContextChangeCost(raw_job_2));
}

TEST(Machine, WorkingMachineIsNotWaitingForAJob) {
  static const io::RawMachine kRawMachine = {1, io::MachineState::kWorking};

  Machine machine(kRawMachine, std::make_shared<ContextChanges>());
  EXPECT_FALSE(machine.IsWaitingForAJob());
}

TEST(Machine, DeadMachineIsNotWaitingForAJob) {
  static const io::RawMachine kRawMachine = {1, io::MachineState::kDead};

  Machine machine(kRawMachine, std::make_shared<ContextChanges>());
  EXPECT_FALSE(machine.IsWaitingForAJob());
}

TEST(Machine, SetStateFromWorkingToIdle) {
  static const io::RawMachine kRawMachine = {1, io::MachineState::kWorking};

  Machine machine(kRawMachine, std::make_shared<ContextChanges>());
  EXPECT_FALSE(machine.IsWaitingForAJob());
  machine.SetState(io::MachineState::kIdle);
  EXPECT_TRUE(machine.IsWaitingForAJob());
}

TEST(Machine, SetStateFromIdleToIdleWithAssignedJob) {
  static const io::RawMachine kRawMachine = {1, io::MachineState::kIdle};
  static const io::RawJob kRawJob = io::RawJob();

  Machine machine(kRawMachine, std::make_shared<ContextChanges>());
  EXPECT_TRUE(machine.IsWaitingForAJob());
  machine.AssignJob(kRawJob);
  EXPECT_FALSE(machine.IsWaitingForAJob());
  machine.SetState(io::MachineState::kIdle);
  EXPECT_FALSE(machine.IsWaitingForAJob());
}

}  // namespace lss

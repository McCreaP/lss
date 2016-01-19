#include "oo_input_types/machine.h"

#include <vector>

#include "gtest/gtest.h"
#include "io/test_utils/builders.h"

namespace lss {

TEST(Machine, GetId) {
  static const int kMachineId = 42;
  static const io::Machine kRawMachine = {kMachineId, io::MachineState::kIdle};

  Machine machine(kRawMachine);
  EXPECT_EQ(kMachineId, machine.GetId());
}

TEST(Machine, AssignJob) {
  static const io::Machine kRawMachine = {1, io::MachineState::kIdle};
  static const io::Job kRawJob1 = io::JobBuilder()
      .WithId(1)
      .WithContext(std::vector<int>({3, 5, 8}))
      .Build();
  static const io::Job kRawJob2 = io::JobBuilder()
      .WithId(2)
      .WithContext(std::vector<int>({3, 999, 999}))
      .Build();
  static const int kContextChangeCost = 42;
  static const io::ContextChange kRawContextChange = {0, 1, 1, kContextChangeCost};
  Machine::SetContextChanges(std::vector<io::ContextChange>({kRawContextChange}));

  Machine machine(kRawMachine);
  EXPECT_TRUE(machine.IsWaitingForAJob());
  machine.AssignJob(kRawJob1);
  EXPECT_FALSE(machine.IsWaitingForAJob());
  EXPECT_EQ(kContextChangeCost, machine.ContextChangeCost(kRawJob2));
}

TEST(Machine, WorkingMachineIsNotWaitingForAJob) {
  static const io::Machine kRawMachine = {1, io::MachineState::kWorking};

  Machine machine(kRawMachine);
  EXPECT_FALSE(machine.IsWaitingForAJob());
}

TEST(Machine, DeadMachineIsNotWaitingForAJob) {
  static const io::Machine kRawMachine = {1, io::MachineState::kDead};

  Machine machine(kRawMachine);
  EXPECT_FALSE(machine.IsWaitingForAJob());
}

TEST(Machine, SetStateFromWorkingToIdle) {
  static const io::Machine kRawMachine = {1, io::MachineState::kWorking};

  Machine machine(kRawMachine);
  EXPECT_FALSE(machine.IsWaitingForAJob());
  machine.SetState(io::MachineState::kIdle);
  EXPECT_TRUE(machine.IsWaitingForAJob());
}

TEST(Machine, SetStateFromIdleToIdleWithAssignedJob) {
  static const io::Machine kRawMachine = {1, io::MachineState::kIdle};
  static const io::Job kRawJob = io::JobBuilder().Build();

  Machine machine(kRawMachine);
  EXPECT_TRUE(machine.IsWaitingForAJob());
  machine.AssignJob(kRawJob);
  EXPECT_FALSE(machine.IsWaitingForAJob());
  machine.SetState(io::MachineState::kIdle);
  EXPECT_FALSE(machine.IsWaitingForAJob());
}

}  // namespace lss

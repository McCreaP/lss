#include "greedy/input.h"

#include <memory>
#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "base/raw_situation.h"
#include "greedy/batch_wrapper.h"

using ::testing::SetArgPointee;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::_;

namespace lss {
namespace greedy {

TEST(Input, UpdateSucceed) {
  std::unique_ptr<io::ReaderMock> reader = std::make_unique<io::ReaderMock>();
  EXPECT_CALL(*reader, Read(_)).WillOnce(Return(true));

  Input input(std::move(reader));
  EXPECT_TRUE(input.Update());
  EXPECT_EQ(std::vector<BatchWrapper>(), input.GetBatches());
}

TEST(Input, UpdateReturnsFalseOnReaderFailure) {
  std::unique_ptr<io::ReaderMock> reader = std::make_unique<io::ReaderMock>();
  EXPECT_CALL(*reader, Read(_)).WillOnce(Return(false));

  Input input(std::move(reader));
  EXPECT_FALSE(input.Update());
}

TEST(Input, GetBatches) {
  RawBatch raw_batch_1 = RawBatch();
  raw_batch_1.id_ = 1;
  RawBatch raw_batch_2 = RawBatch();
  raw_batch_2.id_ = 2;
  static const BatchWrapper kBatch1 = BatchWrapper(raw_batch_1);
  static const BatchWrapper kBatch2 = BatchWrapper(raw_batch_2);
  RawSituation raw_data = RawSituation();
  raw_data.batches_ = {raw_batch_1, raw_batch_2};
  std::unique_ptr<io::ReaderMock> reader = std::make_unique<io::ReaderMock>();
  EXPECT_CALL(*reader, Read(_)).WillOnce(DoAll(SetArgPointee<0>(raw_data), Return(true)));

  Input input(std::move(reader));
  EXPECT_TRUE(input.Update());
  std::vector<BatchWrapper> batches = input.GetBatches();
  EXPECT_EQ(2, batches.size());
  EXPECT_TRUE(kBatch1 == batches[0] || kBatch1 == batches[1]);
  EXPECT_TRUE(kBatch2 == batches[0] || kBatch2 == batches[1]);
}

TEST(Input, GetBatchesAfterUpdate) {
  RawBatch raw_batch_1 = RawBatch();
  raw_batch_1.id_ = 1;
  RawBatch raw_batch_2 = RawBatch();
  raw_batch_2.id_ = 2;
  RawBatch raw_batch_3 = RawBatch();
  raw_batch_3.id_ = 3;
  static const BatchWrapper kBatch1 = BatchWrapper(raw_batch_1);
  static const BatchWrapper kBatch2 = BatchWrapper(raw_batch_2);
  static const BatchWrapper kBatch3 = BatchWrapper(raw_batch_3);
  RawSituation raw_data_1 = RawSituation();
  raw_data_1.batches_ = {raw_batch_1, raw_batch_2};
  RawSituation raw_data_2 = RawSituation();
  raw_data_2.batches_ = {raw_batch_2, raw_batch_3};
  std::unique_ptr<io::ReaderMock> reader = std::make_unique<io::ReaderMock>();
  EXPECT_CALL(*reader, Read(_)).WillOnce(DoAll(SetArgPointee<0>(raw_data_1), Return(true)))
      .WillOnce(DoAll(SetArgPointee<0>(raw_data_2), Return(true)));

  Input input(std::move(reader));
  EXPECT_TRUE(input.Update());
  std::vector<BatchWrapper> batches = input.GetBatches();
  EXPECT_EQ(2, batches.size());
  EXPECT_TRUE(kBatch1 == batches[0] || kBatch1 == batches[1]);
  EXPECT_TRUE(kBatch2 == batches[0] || kBatch2 == batches[1]);

  EXPECT_TRUE(input.Update());
  batches = input.GetBatches();
  EXPECT_EQ(2, batches.size());
  EXPECT_TRUE(kBatch2 == batches[0] || kBatch2 == batches[1]);
  EXPECT_TRUE(kBatch3 == batches[0] || kBatch3 == batches[1]);
}

TEST(Input, EmptyMachineSet) {
  static const auto kRawMachineSet = RawMachineSet().id(1);
  RawSituation raw_data = RawSituation();
  raw_data.machine_sets_ = {kRawMachineSet};
  std::unique_ptr<io::ReaderMock> reader = std::make_unique<io::ReaderMock>();
  EXPECT_CALL(*reader, Read(_)).WillOnce(DoAll(SetArgPointee<0>(raw_data), Return(true)));

  Input input(std::move(reader));
  EXPECT_TRUE(input.Update());
  EXPECT_EQ(std::vector<std::shared_ptr<MachineWrapper>>(), input.GetMachinesFromSet(1));
}

TEST(Input, SingleMachineSet) {
  static const auto kRawMachine = RawMachine().id(1).state(MachineState::kIdle);
  static const auto kRawMachineSet = RawMachineSet().id(1).add(1);
  RawSituation raw_data = RawSituation();
  raw_data.machines_ = {kRawMachine};
  raw_data.machine_sets_ = {kRawMachineSet};
  std::unique_ptr<io::ReaderMock> reader = std::make_unique<io::ReaderMock>();
  EXPECT_CALL(*reader, Read(_)).WillOnce(DoAll(SetArgPointee<0>(raw_data), Return(true)));
  static const MachineWrapper kMachine(kRawMachine, std::make_shared<ContextChanges>());

  Input input(std::move(reader));
  EXPECT_TRUE(input.Update());
  auto machines = input.GetMachinesFromSet(1);
  EXPECT_EQ(1, machines.size());
  EXPECT_EQ(kMachine, *machines[0]);
}

TEST(Input, MultipleMachineSets) {
  static const auto kRawMachine1 = RawMachine().id(1).state(MachineState::kIdle);
  static const auto kRawMachine2 = RawMachine().id(2).state(MachineState::kWorking);
  static const auto kRawMachine3 = RawMachine().id(3).state(MachineState::kDead);
  static const auto kRawMachineSet1 = RawMachineSet().id(1).add(1).add(2);
  static const auto kRawMachineSet2 = RawMachineSet().id(2).add(3);
  RawSituation raw_data = RawSituation();
  raw_data.machines_ = {kRawMachine1, kRawMachine2, kRawMachine3};
  raw_data.machine_sets_ = {kRawMachineSet1, kRawMachineSet2};
  std::unique_ptr<io::ReaderMock> reader = std::make_unique<io::ReaderMock>();
  EXPECT_CALL(*reader, Read(_)).WillOnce(DoAll(SetArgPointee<0>(raw_data), Return(true)));
  static const MachineWrapper kMachine1(kRawMachine1, std::make_shared<ContextChanges>());
  static const MachineWrapper kMachine2(kRawMachine2, std::make_shared<ContextChanges>());
  static const MachineWrapper kMachine3(kRawMachine3, std::make_shared<ContextChanges>());

  Input input(std::move(reader));
  EXPECT_TRUE(input.Update());
  auto machines = input.GetMachinesFromSet(1);
  EXPECT_EQ(2, machines.size());
  EXPECT_TRUE((kMachine1 == *machines[0]) || (kMachine1 == *machines[1]));
  EXPECT_TRUE((kMachine2 == *machines[0]) || (kMachine2 == *machines[1]));
  machines = input.GetMachinesFromSet(2);
  EXPECT_EQ(1, machines.size());
  EXPECT_EQ(kMachine3, *machines[0]);
}

TEST(Input, KeepsOldMachine) {
  static const auto kRawMachine = RawMachine().id(1).state(MachineState::kIdle);
  static const auto kRawMachineSet = RawMachineSet().id(1).add(1);
  RawSituation raw_data = RawSituation();
  raw_data.machines_ = {kRawMachine};
  raw_data.machine_sets_ = {kRawMachineSet};
  std::unique_ptr<io::ReaderMock> reader = std::make_unique<io::ReaderMock>();
  EXPECT_CALL(*reader, Read(_)).WillRepeatedly(DoAll(SetArgPointee<0>(raw_data), Return(true)));
  static const MachineWrapper kMachine(kRawMachine, std::make_shared<ContextChanges>());

  Input input(std::move(reader));
  EXPECT_TRUE(input.Update());
  auto machines = input.GetMachinesFromSet(1);
  EXPECT_EQ(1, machines.size());
  std::shared_ptr<MachineWrapper> old_machine = machines[0];
  EXPECT_EQ(kMachine, *old_machine);

  EXPECT_TRUE(input.Update());
  machines = input.GetMachinesFromSet(1);
  EXPECT_EQ(1, machines.size());
  EXPECT_EQ(old_machine, machines[0]);
}

TEST(Input, AssignOneJob) {
  static const int kAssignedJobId = 42;
  RawJob kRawJob = RawJob();
  kRawJob.id_ = kAssignedJobId;
  static const auto kRawMachine1 = RawMachine().id(10).state(MachineState::kIdle);
  MachineWrapper machine = MachineWrapper(kRawMachine1, std::make_shared<ContextChanges>());
  std::unique_ptr<io::ReaderMock> reader = std::make_unique<io::ReaderMock>();

  Input input(std::move(reader));
  EXPECT_FALSE(input.IsJobAssigned(kAssignedJobId));
  input.Assign(kRawJob, &machine);
  EXPECT_TRUE(input.IsJobAssigned(kAssignedJobId));
  EXPECT_FALSE(input.IsJobAssigned(10));
}

TEST(Input, AssignedJobsAfterUpdate) {
  static const int kFirstJobId = 10;
  static const int kSecondJobId = 20;
  static const RawBatch kRawBatch = RawBatch();
  RawJob raw_job_1 = RawJob();
  raw_job_1.id_ = kFirstJobId;
  RawJob raw_job_2 = RawJob();
  raw_job_2.id_ = kSecondJobId;
  RawSituation raw_data = RawSituation();
  raw_data.jobs_ = {raw_job_2};
  raw_data.batches_ = {kRawBatch};
  static const auto kRawMachine1 = RawMachine().id(10).state(MachineState::kIdle);
  static const auto kRawMachine2 = RawMachine().id(20).state(MachineState::kIdle);
  MachineWrapper machine1 = MachineWrapper(kRawMachine1, std::make_shared<ContextChanges>());
  MachineWrapper machine2 = MachineWrapper(kRawMachine2, std::make_shared<ContextChanges>());
  std::unique_ptr<io::ReaderMock> reader = std::make_unique<io::ReaderMock>();
  EXPECT_CALL(*reader, Read(_)).WillOnce(DoAll(SetArgPointee<0>(raw_data), Return(true)));

  Input input(std::move(reader));
  EXPECT_FALSE(input.IsJobAssigned(kFirstJobId));
  EXPECT_FALSE(input.IsJobAssigned(kSecondJobId));
  input.Assign(raw_job_1, &machine1);
  input.Assign(raw_job_2, &machine2);
  EXPECT_TRUE(input.IsJobAssigned(kFirstJobId));
  EXPECT_TRUE(input.IsJobAssigned(kSecondJobId));
  input.Update();
  EXPECT_FALSE(input.IsJobAssigned(kFirstJobId));
  EXPECT_TRUE(input.IsJobAssigned(kSecondJobId));
}

}  // namespace greedy
}  // namespace lss

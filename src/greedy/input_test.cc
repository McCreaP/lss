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
  io::RawBatch raw_batch_1 = io::RawBatch();
  raw_batch_1.id = 1;
  io::RawBatch raw_batch_2 = io::RawBatch();
  raw_batch_2.id = 2;
  static const BatchWrapper kBatch1 = BatchWrapper(raw_batch_1);
  static const BatchWrapper kBatch2 = BatchWrapper(raw_batch_2);
  io::RawSituation raw_data = io::RawSituation();
  raw_data.batches = {raw_batch_1, raw_batch_2};
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
  io::RawBatch raw_batch_1 = io::RawBatch();
  raw_batch_1.id = 1;
  io::RawBatch raw_batch_2 = io::RawBatch();
  raw_batch_2.id = 2;
  io::RawBatch raw_batch_3 = io::RawBatch();
  raw_batch_3.id = 3;
  static const BatchWrapper kBatch1 = BatchWrapper(raw_batch_1);
  static const BatchWrapper kBatch2 = BatchWrapper(raw_batch_2);
  static const BatchWrapper kBatch3 = BatchWrapper(raw_batch_3);
  io::RawSituation raw_data_1 = io::RawSituation();
  raw_data_1.batches = {raw_batch_1, raw_batch_2};
  io::RawSituation raw_data_2 = io::RawSituation();
  raw_data_2.batches = {raw_batch_2, raw_batch_3};
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
  static const io::RawMachineSet kRawMachineSet = {1, std::vector<int>()};
  io::RawSituation raw_data = io::RawSituation();
  raw_data.machine_sets = {kRawMachineSet};
  std::unique_ptr<io::ReaderMock> reader = std::make_unique<io::ReaderMock>();
  EXPECT_CALL(*reader, Read(_)).WillOnce(DoAll(SetArgPointee<0>(raw_data), Return(true)));

  Input input(std::move(reader));
  EXPECT_TRUE(input.Update());
  EXPECT_EQ(std::vector<std::shared_ptr<MachineWrapper>>(), input.GetMachinesFromSet(1));
}

TEST(Input, SingleMachineSet) {
  static const io::RawMachine kRawMachine = {1, io::MachineState::kIdle, {-1, -1, -1}};
  static const io::RawMachineSet kRawMachineSet = {1, std::vector<int>({1})};
  io::RawSituation raw_data = io::RawSituation();
  raw_data.machines = {kRawMachine};
  raw_data.machine_sets = {kRawMachineSet};
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
  static const io::RawMachine kRawMachine1 = {1, io::MachineState::kIdle, {-1, -1, -1}};
  static const io::RawMachine kRawMachine2 = {2, io::MachineState::kWorking, {-1, -1, -1}};
  static const io::RawMachine kRawMachine3 = {3, io::MachineState::kDead, {-1, -1, -1}};
  static const io::RawMachineSet kRawMachineSet1 = {1, std::vector<int>({1, 2})};
  static const io::RawMachineSet kRawMachineSet2 = {2, std::vector<int>({3})};
  io::RawSituation raw_data = io::RawSituation();
  raw_data.machines = {kRawMachine1, kRawMachine2, kRawMachine3};
  raw_data.machine_sets = {kRawMachineSet1, kRawMachineSet2};
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
  static const io::RawMachine kRawMachine = {1, io::MachineState::kIdle, {-1, -1, -1}};
  static const io::RawMachineSet kRawMachineSet = {1, std::vector<int>({1})};
  io::RawSituation raw_data = io::RawSituation();
  raw_data.machines = {kRawMachine};
  raw_data.machine_sets = {kRawMachineSet};
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
  io::RawJob kRawJob = io::RawJob();
  kRawJob.id = kAssignedJobId;
  static const io::RawMachine kRawMachine1 = {10, io::MachineState::kIdle, {-1, -1, -1}};
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
  static const io::RawBatch kRawBatch = io::RawBatch();
  io::RawJob raw_job_1 = io::RawJob();
  raw_job_1.id = kFirstJobId;
  io::RawJob raw_job_2 = io::RawJob();
  raw_job_2.id = kSecondJobId;
  io::RawSituation raw_data = io::RawSituation();
  raw_data.jobs = {raw_job_2};
  raw_data.batches = {kRawBatch};
  static const io::RawMachine kRawMachine1 = {10, io::MachineState::kIdle, {-1, -1, -1}};
  static const io::RawMachine kRawMachine2 = {20, io::MachineState::kIdle, {-1, -1, -1}};
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

#include "oo_input_types/input.h"

#include <string>
#include <memory>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "io/raw_input_types.h"
#include "oo_input_types/batch.h"

using ::testing::SetArgPointee;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::_;

namespace lss {
namespace io {

class ReaderMock: public Reader {
 public:
  MOCK_METHOD1(SetInputPath, void(const std::string&));
  MOCK_METHOD1(Read, bool(RawData*));
};

}  // namespace io

TEST(Input, UpdateSucceed) {
  std::shared_ptr<io::ReaderMock> reader = std::make_shared<io::ReaderMock>();
  EXPECT_CALL(*reader, Read(_)).WillOnce(Return(true));

  Input input(reader);
  EXPECT_TRUE(input.Update());
  EXPECT_EQ(std::vector<Batch>(), input.GetSortedBatches());
}

TEST(Input, UpdateFailed) {
  std::shared_ptr<io::ReaderMock> reader = std::make_shared<io::ReaderMock>();
  EXPECT_CALL(*reader, Read(_)).WillOnce(Return(false));

  Input input(reader);
  EXPECT_FALSE(input.Update());
  EXPECT_EQ(std::vector<Batch>(), input.GetSortedBatches());
}

TEST(Input, TwoBatchesOneEmpty) {
  io::Batch raw_batch_1 = io::Batch();
  raw_batch_1.id = 1;
  io::Batch raw_batch_2 = io::Batch();
  raw_batch_2.id = 2;
  io::Job raw_job = io::Job();
  raw_job.batch_id = 1;
  raw_job.duration = 1;
  io::RawData raw_data = io::RawData();
  raw_data.batches = {raw_batch_1, raw_batch_2};
  raw_data.jobs = {raw_job};

  std::shared_ptr<io::ReaderMock> reader = std::make_shared<io::ReaderMock>();
  EXPECT_CALL(*reader, Read(_)).WillOnce((DoAll(SetArgPointee<0>(raw_data), Return(true))));
  Batch batch1(raw_batch_1);
  Batch batch2(raw_batch_2);
  batch1.AddJob(raw_job);

  Input input(reader);
  EXPECT_TRUE(input.Update());
  EXPECT_EQ(std::vector<Batch>({batch2, batch1}), input.GetSortedBatches());
}

TEST(Input, ThreeBatches) {
  io::Batch raw_batch_1 = io::Batch();
  raw_batch_1.id = 1;
  raw_batch_1.reward = 2.0;
  io::Batch raw_batch_2 = io::Batch();
  raw_batch_2.id = 2;
  raw_batch_2.reward = 3.0;
  io::Batch raw_batch_3 = io::Batch();
  raw_batch_3.id = 3;
  raw_batch_3.reward = 1.0;
  io::Job raw_job_1 = io::Job();
  raw_job_1.batch_id = 1;
  raw_job_1.duration = 1;
  io::Job raw_job_2 = io::Job();
  raw_job_2.batch_id = 2;
  raw_job_2.duration = 1;
  io::Job raw_job_3 = io::Job();
  raw_job_3.batch_id = 3;
  raw_job_3.duration = 1;
  io::RawData raw_data = io::RawData();
  raw_data.batches = {raw_batch_1, raw_batch_2, raw_batch_3};
  raw_data.jobs = {raw_job_1, raw_job_2, raw_job_3};

  std::shared_ptr<io::ReaderMock> reader = std::make_shared<io::ReaderMock>();
  EXPECT_CALL(*reader, Read(_)).WillOnce((DoAll(SetArgPointee<0>(raw_data), Return(true))));
  Batch batch1(raw_batch_1);
  Batch batch2(raw_batch_2);
  Batch batch3(raw_batch_3);
  batch1.AddJob(raw_job_1);
  batch2.AddJob(raw_job_2);
  batch3.AddJob(raw_job_3);

  Input input(reader);
  EXPECT_TRUE(input.Update());
  EXPECT_EQ(std::vector<Batch>({batch3, batch1, batch2}), input.GetSortedBatches());
}

TEST(Input, EmptyMachineSet) {
  static const io::MachineSet kRawMachineSet = {1, std::vector<int>()};
  io::RawData raw_data = io::RawData();
  raw_data.machine_sets = {kRawMachineSet};
  std::shared_ptr<io::ReaderMock> reader = std::make_shared<io::ReaderMock>();
  EXPECT_CALL(*reader, Read(_)).WillOnce((DoAll(SetArgPointee<0>(raw_data), Return(true))));

  Input input(reader);
  EXPECT_TRUE(input.Update());
  EXPECT_EQ(std::vector<std::shared_ptr<Machine>>(), input.GetMachinesFromSet(1));
}

TEST(Input, SingleMachineSet) {
  static const io::Machine kRawMachine = {1, io::MachineState::kIdle};
  static const io::MachineSet kRawMachineSet = {1, std::vector<int>({1})};
  io::RawData raw_data = io::RawData();
  raw_data.machines = {kRawMachine};
  raw_data.machine_sets = {kRawMachineSet};
  std::shared_ptr<io::ReaderMock> reader = std::make_shared<io::ReaderMock>();
  EXPECT_CALL(*reader, Read(_)).WillOnce((DoAll(SetArgPointee<0>(raw_data), Return(true))));
  static const Machine kMachine(kRawMachine, std::make_shared<ContextChanges>());

  Input input(reader);
  EXPECT_TRUE(input.Update());
  auto machines = input.GetMachinesFromSet(1);
  EXPECT_EQ(1, machines.size());
  EXPECT_EQ(kMachine, *machines[0]);
}

TEST(Input, MultipleMachineSets) {
  static const io::Machine kRawMachine1 = {1, io::MachineState::kIdle};
  static const io::Machine kRawMachine2 = {2, io::MachineState::kWorking};
  static const io::Machine kRawMachine3 = {3, io::MachineState::kDead};
  static const io::MachineSet kRawMachineSet1 = {1, std::vector<int>({1, 2})};
  static const io::MachineSet kRawMachineSet2 = {2, std::vector<int>({3})};
  io::RawData raw_data = io::RawData();
  raw_data.machines = {kRawMachine1, kRawMachine2, kRawMachine3};
  raw_data.machine_sets = {kRawMachineSet1, kRawMachineSet2};
  std::shared_ptr<io::ReaderMock> reader = std::make_shared<io::ReaderMock>();
  EXPECT_CALL(*reader, Read(_)).WillOnce((DoAll(SetArgPointee<0>(raw_data), Return(true))));
  static const Machine kMachine1(kRawMachine1, std::make_shared<ContextChanges>());
  static const Machine kMachine2(kRawMachine2, std::make_shared<ContextChanges>());
  static const Machine kMachine3(kRawMachine3, std::make_shared<ContextChanges>());

  Input input(reader);
  EXPECT_TRUE(input.Update());
  auto machines = input.GetMachinesFromSet(1);
  EXPECT_EQ(2, machines.size());
  EXPECT_EQ(kMachine1, *machines[0]);
  EXPECT_EQ(kMachine2, *machines[1]);
  machines = input.GetMachinesFromSet(2);
  EXPECT_EQ(1, machines.size());
  EXPECT_EQ(kMachine3, *machines[0]);
}

TEST(Input, KeepsOldMachine) {
  static const io::Machine kRawMachine = {1, io::MachineState::kIdle};
  static const io::MachineSet kRawMachineSet = {1, std::vector<int>({1})};
  io::RawData raw_data = io::RawData();
  raw_data.machines = {kRawMachine};
  raw_data.machine_sets = {kRawMachineSet};
  std::shared_ptr<io::ReaderMock> reader = std::make_shared<io::ReaderMock>();
  EXPECT_CALL(*reader, Read(_)).WillRepeatedly((DoAll(SetArgPointee<0>(raw_data), Return(true))));
  static const Machine kMachine(kRawMachine, std::make_shared<ContextChanges>());

  Input input(reader);
  EXPECT_TRUE(input.Update());
  auto machines = input.GetMachinesFromSet(1);
  EXPECT_EQ(1, machines.size());
  std::shared_ptr<Machine> old_machine = machines[0];
  EXPECT_EQ(kMachine, *old_machine);

  EXPECT_TRUE(input.Update());
  machines = input.GetMachinesFromSet(1);
  EXPECT_EQ(1, machines.size());
  EXPECT_EQ(old_machine, machines[0]);
}

TEST(Input, AssignOneJob) {
  static const int kAssignedJobId = 42;
  io::Job kRawJob = io::Job();
  kRawJob.id = kAssignedJobId;
  static const io::Machine kRawMachine1 = {10, io::MachineState::kIdle};
  Machine machine = Machine(kRawMachine1, std::make_shared<ContextChanges>());
  std::shared_ptr<io::ReaderMock> reader = std::make_shared<io::ReaderMock>();

  Input input(reader);
  EXPECT_FALSE(input.IsJobAssigned(kAssignedJobId));
  input.Assign(kRawJob, &machine);
  EXPECT_TRUE(input.IsJobAssigned(kAssignedJobId));
  EXPECT_FALSE(input.IsJobAssigned(10));
}

TEST(Input, AssignedJobsAfterUpdate) {
  static const int kFirstJobId = 10;
  static const int kSecondJobId = 20;
  static const io::Batch kRawBatch = io::Batch();
  io::Job raw_job_1 = io::Job();
  raw_job_1.id = kFirstJobId;
  io::Job raw_job_2 = io::Job();
  raw_job_2.id = kSecondJobId;
  io::RawData raw_data = io::RawData();
  raw_data.jobs = {raw_job_2};
  raw_data.batches = {kRawBatch};
  static const io::Machine kRawMachine1 = {10, io::MachineState::kIdle};
  static const io::Machine kRawMachine2 = {20, io::MachineState::kIdle};
  Machine machine1 = Machine(kRawMachine1, std::make_shared<ContextChanges>());
  Machine machine2 = Machine(kRawMachine2, std::make_shared<ContextChanges>());
  std::shared_ptr<io::ReaderMock> reader = std::make_shared<io::ReaderMock>();
  EXPECT_CALL(*reader, Read(_)).WillOnce((DoAll(SetArgPointee<0>(raw_data), Return(true))));

  Input input(reader);
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

}  // namespace lss

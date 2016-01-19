#include "oo_input_types/input.h"

#include <string>
#include <memory>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "io/raw_input_types.h"
#include "io/test_utils/builders.h"
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
  static const io::Batch kRawBatch1 = io::BatchBuilder().WithId(1).Build();
  static const io::Batch kRawBatch2 = io::BatchBuilder().WithId(2).Build();
  static const io::Job kJob = io::JobBuilder().WithBatchId(1).Build();
  static io::RawData kRawData = io::RawDataBuilder()
      .WithBatches(std::vector<io::Batch>({kRawBatch1, kRawBatch2}))
      .WithJob(kJob)
      .Build();
  std::shared_ptr<io::ReaderMock> reader = std::make_shared<io::ReaderMock>();
  EXPECT_CALL(*reader, Read(_)).WillOnce((DoAll(SetArgPointee<0>(kRawData), Return(true))));
  Batch batch1(kRawBatch1);
  Batch batch2(kRawBatch2);
  batch1.AddJob(kJob);

  Input input(reader);
  EXPECT_TRUE(input.Update());
  EXPECT_EQ(std::vector<Batch>({batch2, batch1}), input.GetSortedBatches());
}

TEST(Input, ThreeBatches) {
  static const io::Batch kRawBatch1 = io::BatchBuilder().WithId(1).WithReward(2.0).Build();
  static const io::Batch kRawBatch2 = io::BatchBuilder().WithId(2).WithReward(3.0).Build();
  static const io::Batch kRawBatch3 = io::BatchBuilder().WithId(3).WithReward(1.0).Build();
  static const io::Job kJob1 = io::JobBuilder().WithBatchId(1).Build();
  static const io::Job kJob2 = io::JobBuilder().WithBatchId(2).Build();
  static const io::Job kJob3 = io::JobBuilder().WithBatchId(3).Build();
  static io::RawData kRawData = io::RawDataBuilder()
      .WithBatches(std::vector<io::Batch>({kRawBatch1, kRawBatch2, kRawBatch3}))
      .WithJobs(std::vector<io::Job>({kJob1, kJob2, kJob3}))
      .Build();
  std::shared_ptr<io::ReaderMock> reader = std::make_shared<io::ReaderMock>();
  EXPECT_CALL(*reader, Read(_)).WillOnce((DoAll(SetArgPointee<0>(kRawData), Return(true))));
  Batch batch1(kRawBatch1);
  Batch batch2(kRawBatch2);
  Batch batch3(kRawBatch3);
  batch1.AddJob(kJob1);
  batch2.AddJob(kJob2);
  batch3.AddJob(kJob3);

  Input input(reader);
  EXPECT_TRUE(input.Update());
  EXPECT_EQ(std::vector<Batch>({batch3, batch1, batch2}), input.GetSortedBatches());
}

TEST(Input, EmptyMachineSet) {
  static const io::MachineSet kRawMachineSet = {1, std::vector<int>()};
  static io::RawData kRawData = io::RawDataBuilder()
      .WithMachineSet(kRawMachineSet)
      .Build();
  std::shared_ptr<io::ReaderMock> reader = std::make_shared<io::ReaderMock>();
  EXPECT_CALL(*reader, Read(_)).WillOnce((DoAll(SetArgPointee<0>(kRawData), Return(true))));

  Input input(reader);
  EXPECT_TRUE(input.Update());
  EXPECT_EQ(std::vector<std::shared_ptr<Machine>>(), input.GetMachinesFromSet(1));
}

TEST(Input, SingleMachineSet) {
  static const io::Machine kRawMachine = {1, io::MachineState::kIdle};
  static const io::MachineSet kRawMachineSet = {1, std::vector<int>({1})};
  static io::RawData kRawData = io::RawDataBuilder()
      .WithMachine(kRawMachine)
      .WithMachineSet(kRawMachineSet)
      .Build();
  std::shared_ptr<io::ReaderMock> reader = std::make_shared<io::ReaderMock>();
  EXPECT_CALL(*reader, Read(_)).WillOnce((DoAll(SetArgPointee<0>(kRawData), Return(true))));
  static const Machine kMachine(kRawMachine);

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
  static io::RawData kRawData = io::RawDataBuilder()
      .WithMachines(std::vector<io::Machine>({kRawMachine1, kRawMachine2, kRawMachine3}))
      .WithMachineSets(std::vector<io::MachineSet>({kRawMachineSet1, kRawMachineSet2}))
      .Build();
  std::shared_ptr<io::ReaderMock> reader = std::make_shared<io::ReaderMock>();
  EXPECT_CALL(*reader, Read(_)).WillOnce((DoAll(SetArgPointee<0>(kRawData), Return(true))));
  static const Machine kMachine1(kRawMachine1);
  static const Machine kMachine2(kRawMachine2);
  static const Machine kMachine3(kRawMachine3);

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
  static io::RawData kRawData = io::RawDataBuilder()
      .WithMachine(kRawMachine)
      .WithMachineSet(kRawMachineSet)
      .Build();
  std::shared_ptr<io::ReaderMock> reader = std::make_shared<io::ReaderMock>();
  EXPECT_CALL(*reader, Read(_)).WillRepeatedly((DoAll(SetArgPointee<0>(kRawData), Return(true))));
  static const Machine kMachine(kRawMachine);

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
  static const io::Job kRawJob = io::JobBuilder().WithId(kAssignedJobId).Build();
  static const io::Machine kRawMachine1 = {10, io::MachineState::kIdle};
  Machine machine = Machine(kRawMachine1);
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
  static const io::Batch kRawBatch = io::BatchBuilder().Build();
  static const io::Job kRawJob1 = io::JobBuilder().WithId(kFirstJobId).Build();
  static const io::Job kRawJob2 = io::JobBuilder().WithId(kSecondJobId).Build();
  static const io::RawData kRawData = io::RawDataBuilder()
      .WithJob(kRawJob2)
      .WithBatch(kRawBatch)
      .Build();
  static const io::Machine kRawMachine1 = {10, io::MachineState::kIdle};
  static const io::Machine kRawMachine2 = {20, io::MachineState::kIdle};
  Machine machine1 = Machine(kRawMachine1);
  Machine machine2 = Machine(kRawMachine2);
  std::shared_ptr<io::ReaderMock> reader = std::make_shared<io::ReaderMock>();
  EXPECT_CALL(*reader, Read(_)).WillOnce((DoAll(SetArgPointee<0>(kRawData), Return(true))));

  Input input(reader);
  EXPECT_FALSE(input.IsJobAssigned(kFirstJobId));
  EXPECT_FALSE(input.IsJobAssigned(kSecondJobId));
  input.Assign(kRawJob1, &machine1);
  input.Assign(kRawJob2, &machine2);
  EXPECT_TRUE(input.IsJobAssigned(kFirstJobId));
  EXPECT_TRUE(input.IsJobAssigned(kSecondJobId));
  input.Update();
  EXPECT_FALSE(input.IsJobAssigned(kFirstJobId));
  EXPECT_TRUE(input.IsJobAssigned(kSecondJobId));
}

}  // namespace lss

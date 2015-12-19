#include "oo_input_types/batch.h"

#include <io/raw_input_types.h>
#include <io/test_utils/builders.h>
#include "gtest/gtest.h"

namespace lss {

TEST(Batch, JobCmpRetrunsTrue) {
  static const io::Job kRawJob1 = io::JobBuilder().WithId(1).WithDuration(1.5).Build();
  static const io::Job kRawJob2 = io::JobBuilder().WithId(2).WithDuration(2.0).Build();

  EXPECT_EQ(true, JobCmp()(kRawJob1, kRawJob2));
}

TEST(Batch, JobCmpRetrunsFalse) {
  static const io::Job kRawJob1 = io::JobBuilder().WithId(1).WithDuration(1.99).Build();
  static const io::Job kRawJob2 = io::JobBuilder().WithId(2).WithDuration(1.98).Build();

  EXPECT_EQ(false, JobCmp()(kRawJob1, kRawJob2));
}

TEST(Batch, WithNoJobs) {
  static const io::Batch kRawBatch = io::BatchBuilder().Build();
  Batch batch(kRawBatch);
  EXPECT_EQ(0, batch.GetSortedJobs().size());
}

TEST(Batch, GetMultipleJobs) {
  static const io::Batch kRawBatch = io::BatchBuilder().Build();
  static const io::Job kRawJob1 = io::JobBuilder().WithId(1).WithDuration(2.2).Build();
  static const io::Job kRawJob2 = io::JobBuilder().WithId(2).WithDuration(1.1).Build();
  static const io::Job kRawJob3 = io::JobBuilder().WithId(3).WithDuration(3.3).Build();
  Batch batch(kRawBatch);
  batch.AddJob(kRawJob1);
  batch.AddJob(kRawJob2);
  batch.AddJob(kRawJob3);

  static const std::set<io::Job, JobCmp> kExpectedJobs{kRawJob2, kRawJob1, kRawJob3};
  auto sorted_jobs = batch.GetSortedJobs();
  EXPECT_EQ(kExpectedJobs, sorted_jobs);
}

TEST(Batch, GetId) {
  static const int kBatchId = 42;
  static const io::Batch kRawBatch = io::BatchBuilder().WithId(kBatchId).Build();
  Batch batch(kRawBatch);
  EXPECT_EQ(kBatchId, batch.GetId());
}

TEST(Batch, BatchCompareOperator) {
  static const std::time_t kTime = 1450000000;
  static const io::Batch kRawBatch1 = io::BatchBuilder()
      .WithId(1)
      .WithDue(kTime + 350)
      .WithExpectedTime(1800)
      .WithReward(1)
      .WithTimelyReward(10)
      .Build();
  static const io::Batch kRawBatch2 = io::BatchBuilder()
      .WithId(2)
      .WithDue(kTime + 500)
      .WithExpectedTime(1300)
      .WithReward(3)
      .WithTimelyReward(7)
      .Build();
  static const io::Batch kRawBatch3 = io::BatchBuilder()
      .WithId(3)
      .WithDue(kTime + 10000)
      .WithExpectedTime(15000)
      .WithReward(3)
      .WithTimelyReward(100)
      .Build();
  static const io::Batch kRawBatch4 = io::BatchBuilder()
      .WithId(4)
      .WithDue(kTime - 700)
      .WithExpectedTime(1800)
      .WithReward(3)
      .WithTimelyReward(10)
      .Build();
  static const io::Batch kRawBatch5 = io::BatchBuilder()
      .WithId(5)
      .WithDue(kTime + 1000000000)
      .WithExpectedTime(1)
      .WithReward(1000000000)
      .WithTimelyReward(1000000000)
      .Build();

  Batch batch1(kRawBatch1);
  Batch batch2(kRawBatch2);
  Batch batch3(kRawBatch3);
  Batch batch4(kRawBatch4);
  Batch batch5(kRawBatch5);
  static const double kPrecision = 0.0001;
  EXPECT_NEAR(6.48459, batch1.CurrentReward(kTime), kPrecision);
  EXPECT_NEAR(7.16490, batch2.CurrentReward(kTime), kPrecision);
  EXPECT_NEAR(69.07564, batch3.CurrentReward(kTime), kPrecision);
  EXPECT_NEAR(7.03985, batch4.CurrentReward(kTime), kPrecision);
  EXPECT_NEAR(2000000000, batch5.CurrentReward(kTime), kPrecision);
}

}  // namespace lss

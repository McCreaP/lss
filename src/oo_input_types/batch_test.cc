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
}

}  // namespace lss

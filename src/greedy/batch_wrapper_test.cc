#include "greedy/batch_wrapper.h"

#include <algorithm>
#include <vector>

#include "gtest/gtest.h"

#include "base/raw_situation.h"

namespace lss {
namespace greedy {

TEST(BatchWrapper, JobCmpRetrunsTrue) {
  RawJob raw_job_1 = RawJob();
  raw_job_1.id_ = 1;
  raw_job_1.duration_ = 1.5;
  RawJob raw_job_2 = RawJob();
  raw_job_2.id_ = 2;
  raw_job_2.duration_ = 2.0;

  EXPECT_TRUE(JobDurationCmp()(raw_job_1, raw_job_2));
}

TEST(BatchWrapper, JobCmpRetrunsFalse) {
  RawJob raw_job_1 = RawJob();
  raw_job_1.id_ = 1;
  raw_job_1.duration_ = 1.99;
  RawJob raw_job_2 = RawJob();
  raw_job_2.id_ = 2;
  raw_job_2.duration_ = 1.98;

  EXPECT_FALSE(JobDurationCmp()(raw_job_1, raw_job_2));
}

TEST(BatchWrapper, WithNoJobs) {
  static const RawBatch kRawBatch = RawBatch();
  BatchWrapper batch(kRawBatch);
  EXPECT_EQ(0, batch.GetSortedJobs().size());
}

TEST(BatchWrapper, GetMultipleJobs) {
  RawJob raw_job_1 = RawJob();
  raw_job_1.id_ = 1;
  raw_job_1.duration_ = 2.2;
  RawJob raw_job_2 = RawJob();
  raw_job_2.id_ = 2;
  raw_job_2.duration_ = 1.1;
  RawJob raw_job_3 = RawJob();
  raw_job_3.id_ = 3;
  raw_job_3.duration_ = 3.3;
  static const RawBatch kRawBatch = RawBatch();
  BatchWrapper batch(kRawBatch);
  batch.AddJob(raw_job_1);
  batch.AddJob(raw_job_2);
  batch.AddJob(raw_job_3);

  static const std::set<RawJob, JobDurationCmp> kExpectedJobs{raw_job_2, raw_job_1, raw_job_3};
  auto sorted_jobs = batch.GetSortedJobs();
  EXPECT_EQ(kExpectedJobs, sorted_jobs);
}

TEST(BatchWrapper, GetId) {
  static const int kBatchId = 42;
  RawBatch raw_batch = RawBatch();
  raw_batch.id_ = 42;
  BatchWrapper batch(raw_batch);
  EXPECT_EQ(kBatchId, batch.GetId());
}

TEST(BatchWrapper, BatchCompareOperator) {
  static const std::time_t kTime = 1450000000;
  RawBatch raw_batch_1 = RawBatch();
  raw_batch_1.id_ = 1;
  raw_batch_1.due_ = kTime + 350;
  raw_batch_1.duration_ = 1800;
  raw_batch_1.reward_ = 1;
  raw_batch_1.timely_reward_ = 10;
  RawBatch raw_batch_2 = RawBatch();
  raw_batch_2.id_ = 2;
  raw_batch_2.due_ = kTime + 500;
  raw_batch_2.duration_ = 1300;
  raw_batch_2.reward_ = 3;
  raw_batch_2.timely_reward_ = 7;
  RawBatch raw_batch_3 = RawBatch();
  raw_batch_3.id_ = 3;
  raw_batch_3.due_ = kTime + 10000;
  raw_batch_3.duration_ = 15000;
  raw_batch_3.reward_ = 3;
  raw_batch_3.timely_reward_ = 100;
  RawBatch raw_batch_4 = RawBatch();
  raw_batch_4.id_ = 4;
  raw_batch_4.due_ = kTime - 700;
  raw_batch_4.duration_ = 1800;
  raw_batch_4.reward_ = 3;
  raw_batch_4.timely_reward_ = 10;
  RawBatch raw_batch_5 = RawBatch();
  raw_batch_5.id_ = 5;
  raw_batch_5.due_ = kTime + 1000000000;
  raw_batch_5.duration_ = 1;
  raw_batch_5.reward_ = 1000000000;
  raw_batch_5.timely_reward_ = 1000000000;

  BatchWrapper batch1(raw_batch_1);
  BatchWrapper batch2(raw_batch_2);
  BatchWrapper batch3(raw_batch_3);
  BatchWrapper batch4(raw_batch_4);
  BatchWrapper batch5(raw_batch_5);
  static const double kPrecision = 0.0001;
  EXPECT_NEAR(6.48459, batch1.RewardAt(kTime), kPrecision);
  EXPECT_NEAR(7.16490, batch2.RewardAt(kTime), kPrecision);
  EXPECT_NEAR(69.07564, batch3.RewardAt(kTime), kPrecision);
  EXPECT_NEAR(7.03985, batch4.RewardAt(kTime), kPrecision);
  EXPECT_NEAR(2000000000, batch5.RewardAt(kTime), kPrecision);
}

TEST(BatchWrapper, SortTwoBatchesOneEmpty) {
  RawBatch raw_batch_1 = RawBatch();
  raw_batch_1.id_ = 1;
  RawBatch raw_batch_2 = RawBatch();
  raw_batch_2.id_ = 2;
  RawJob raw_job = RawJob();
  raw_job.batch_ = 1;
  raw_job.duration_ = 1;
  RawSituation raw_data = RawSituation();
  raw_data.batches_ = {raw_batch_1, raw_batch_2};
  raw_data.jobs_ = {raw_job};

  BatchWrapper batch1(raw_batch_1);
  BatchWrapper batch2(raw_batch_2);
  batch1.AddJob(raw_job);

  std::vector<BatchWrapper> batches = {batch1, batch2};
  std::sort(std::begin(batches), std::end(batches), BatchRewardCmp());
  EXPECT_EQ(std::vector<BatchWrapper>({batch2, batch1}), batches);
}

TEST(Input, SortThreeBatches) {
  RawBatch raw_batch_1 = RawBatch();
  raw_batch_1.id_ = 1;
  raw_batch_1.reward_ = 2.0;
  RawBatch raw_batch_2 = RawBatch();
  raw_batch_2.id_ = 2;
  raw_batch_2.reward_ = 3.0;
  RawBatch raw_batch_3 = RawBatch();
  raw_batch_3.id_ = 3;
  raw_batch_3.reward_ = 1.0;
  RawJob raw_job_1 = RawJob();
  raw_job_1.batch_ = 1;
  raw_job_1.duration_ = 1;
  RawJob raw_job_2 = RawJob();
  raw_job_2.batch_ = 2;
  raw_job_2.duration_ = 1;
  RawJob raw_job_3 = RawJob();
  raw_job_3.batch_ = 3;
  raw_job_3.duration_ = 1;
  RawSituation raw_data = RawSituation();
  raw_data.batches_ = {raw_batch_1, raw_batch_2, raw_batch_3};
  raw_data.jobs_ = {raw_job_1, raw_job_2, raw_job_3};

  BatchWrapper batch1(raw_batch_1);
  BatchWrapper batch2(raw_batch_2);
  BatchWrapper batch3(raw_batch_3);
  batch1.AddJob(raw_job_1);
  batch2.AddJob(raw_job_2);
  batch3.AddJob(raw_job_3);

  std::vector<BatchWrapper> batches = {batch1, batch2, batch3};
  std::sort(std::begin(batches), std::end(batches), BatchRewardCmp());
  EXPECT_EQ(std::vector<BatchWrapper>({batch3, batch1, batch2}), batches);
}

}  // namespace greedy
}  // namespace lss

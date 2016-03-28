#include "greedy/batch_wrapper.h"

#include <algorithm>
#include <vector>

#include "gtest/gtest.h"

#include "base/raw_situation.h"

namespace lss {
namespace greedy {

TEST(BatchWrapper, JobCmpRetrunsTrue) {
  io::RawJob raw_job_1 = io::RawJob();
  raw_job_1.id = 1;
  raw_job_1.duration = 1.5;
  io::RawJob raw_job_2 = io::RawJob();
  raw_job_2.id = 2;
  raw_job_2.duration = 2.0;

  EXPECT_TRUE(JobDurationCmp()(raw_job_1, raw_job_2));
}

TEST(BatchWrapper, JobCmpRetrunsFalse) {
  io::RawJob raw_job_1 = io::RawJob();
  raw_job_1.id = 1;
  raw_job_1.duration = 1.99;
  io::RawJob raw_job_2 = io::RawJob();
  raw_job_2.id = 2;
  raw_job_2.duration = 1.98;

  EXPECT_FALSE(JobDurationCmp()(raw_job_1, raw_job_2));
}

TEST(BatchWrapper, WithNoJobs) {
  static const io::RawBatch kRawBatch = io::RawBatch();
  BatchWrapper batch(kRawBatch);
  EXPECT_EQ(0, batch.GetSortedJobs().size());
}

TEST(BatchWrapper, GetMultipleJobs) {
  io::RawJob raw_job_1 = io::RawJob();
  raw_job_1.id = 1;
  raw_job_1.duration = 2.2;
  io::RawJob raw_job_2 = io::RawJob();
  raw_job_2.id = 2;
  raw_job_2.duration = 1.1;
  io::RawJob raw_job_3 = io::RawJob();
  raw_job_3.id = 3;
  raw_job_3.duration = 3.3;
  static const io::RawBatch kRawBatch = io::RawBatch();
  BatchWrapper batch(kRawBatch);
  batch.AddJob(raw_job_1);
  batch.AddJob(raw_job_2);
  batch.AddJob(raw_job_3);

  static const std::set<io::RawJob, JobDurationCmp> kExpectedJobs{raw_job_2, raw_job_1, raw_job_3};
  auto sorted_jobs = batch.GetSortedJobs();
  EXPECT_EQ(kExpectedJobs, sorted_jobs);
}

TEST(BatchWrapper, GetId) {
  static const int kBatchId = 42;
  io::RawBatch raw_batch = io::RawBatch();
  raw_batch.id = 42;
  BatchWrapper batch(raw_batch);
  EXPECT_EQ(kBatchId, batch.GetId());
}

TEST(BatchWrapper, BatchCompareOperator) {
  static const std::time_t kTime = 1450000000;
  io::RawBatch raw_batch_1 = io::RawBatch();
  raw_batch_1.id = 1;
  raw_batch_1.due = kTime + 350;
  raw_batch_1.expected_time = 1800;
  raw_batch_1.reward = 1;
  raw_batch_1.timely_reward = 10;
  io::RawBatch raw_batch_2 = io::RawBatch();
  raw_batch_2.id = 2;
  raw_batch_2.due = kTime + 500;
  raw_batch_2.expected_time = 1300;
  raw_batch_2.reward = 3;
  raw_batch_2.timely_reward = 7;
  io::RawBatch raw_batch_3 = io::RawBatch();
  raw_batch_3.id = 3;
  raw_batch_3.due = kTime + 10000;
  raw_batch_3.expected_time = 15000;
  raw_batch_3.reward = 3;
  raw_batch_3.timely_reward = 100;
  io::RawBatch raw_batch_4 = io::RawBatch();
  raw_batch_4.id = 4;
  raw_batch_4.due = kTime - 700;
  raw_batch_4.expected_time = 1800;
  raw_batch_4.reward = 3;
  raw_batch_4.timely_reward = 10;
  io::RawBatch raw_batch_5 = io::RawBatch();
  raw_batch_5.id = 5;
  raw_batch_5.due = kTime + 1000000000;
  raw_batch_5.expected_time = 1;
  raw_batch_5.reward = 1000000000;
  raw_batch_5.timely_reward = 1000000000;

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
  io::RawBatch raw_batch_1 = io::RawBatch();
  raw_batch_1.id = 1;
  io::RawBatch raw_batch_2 = io::RawBatch();
  raw_batch_2.id = 2;
  io::RawJob raw_job = io::RawJob();
  raw_job.batch_id = 1;
  raw_job.duration = 1;
  io::RawSituation raw_data = io::RawSituation();
  raw_data.batches = {raw_batch_1, raw_batch_2};
  raw_data.jobs = {raw_job};

  BatchWrapper batch1(raw_batch_1);
  BatchWrapper batch2(raw_batch_2);
  batch1.AddJob(raw_job);

  std::vector<BatchWrapper> batches = {batch1, batch2};
  std::sort(std::begin(batches), std::end(batches), BatchRewardCmp());
  EXPECT_EQ(std::vector<BatchWrapper>({batch2, batch1}), batches);
}

TEST(Input, SortThreeBatches) {
  io::RawBatch raw_batch_1 = io::RawBatch();
  raw_batch_1.id = 1;
  raw_batch_1.reward = 2.0;
  io::RawBatch raw_batch_2 = io::RawBatch();
  raw_batch_2.id = 2;
  raw_batch_2.reward = 3.0;
  io::RawBatch raw_batch_3 = io::RawBatch();
  raw_batch_3.id = 3;
  raw_batch_3.reward = 1.0;
  io::RawJob raw_job_1 = io::RawJob();
  raw_job_1.batch_id = 1;
  raw_job_1.duration = 1;
  io::RawJob raw_job_2 = io::RawJob();
  raw_job_2.batch_id = 2;
  raw_job_2.duration = 1;
  io::RawJob raw_job_3 = io::RawJob();
  raw_job_3.batch_id = 3;
  raw_job_3.duration = 1;
  io::RawSituation raw_data = io::RawSituation();
  raw_data.batches = {raw_batch_1, raw_batch_2, raw_batch_3};
  raw_data.jobs = {raw_job_1, raw_job_2, raw_job_3};

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

#include "greedy/batch_wrapper.h"

#include <algorithm>
#include <iostream>
#include <ctime>
#include <cmath>
#include <limits>

#include "base/raw_situation.h"

namespace lss {
namespace greedy {

static constexpr double kMinValue = std::numeric_limits<double>::lowest();

BatchWrapper::BatchWrapper(RawBatch raw_batch) :
    raw_batch_(std::move(raw_batch)), time_to_finish_(0) { }

bool BatchWrapper::operator==(const BatchWrapper& rhs) const {
  return raw_batch_.id_ == rhs.raw_batch_.id_;
}

double BatchWrapper::Evaluate(std::time_t time) const {
  if (!time_to_finish_)
    return kMinValue;
  return RewardAt(time) / time_to_finish_;
}

double BatchWrapper::RewardAt(std::time_t time) const {
  double r = (time - raw_batch_.due_) / raw_batch_.duration_;
  return raw_batch_.reward_ + raw_batch_.timely_reward_ / (1 + exp(r));
}

void BatchWrapper::AddJob(const RawJob& raw_job) {
  time_to_finish_ += raw_job.duration_;
  jobs_.insert(raw_job);
}

int BatchWrapper::GetId() const {
  return raw_batch_.id_;
}

const std::set<RawJob, JobDurationCmp>& BatchWrapper::GetSortedJobs() const {
  return jobs_;
}

}  // namespace greedy
}  // namespace lss

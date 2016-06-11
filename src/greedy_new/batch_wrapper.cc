#include "greedy_new/batch_wrapper.h"

#include <algorithm>
#include <iostream>
#include <ctime>
#include <cmath>
#include <limits>

#include "base/raw_situation.h"

namespace lss {
namespace greedy_new {

static constexpr double kMinValue = std::numeric_limits<double>::lowest();

BatchWrapper::BatchWrapper(Batch batch) : batch_(batch) {
  time_to_finish_ = 0;
  for (Job job : batch.jobs()) {
    time_to_finish_ += job.duration();
    jobs_.insert(job);
  }
}

double BatchWrapper::Evaluate(std::time_t time) const {
  if (!time_to_finish_)
    return kMinValue;
  return RewardAt(time) / time_to_finish_;
}

double BatchWrapper::RewardAt(std::time_t time) const {
  double r = (time - batch_.due()) / batch_.duration();
  return batch_.reward() + batch_.timely_reward() / (1 + exp(r));
}

const std::set<Job, JobDurationCmp>& BatchWrapper::GetSortedJobs() const {
  return jobs_;
}

}  // namespace greedy_new
}  // namespace lss

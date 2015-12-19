#include "oo_input_types/batch.h"

#include <algorithm>
#include <iostream>
#include <ctime>
#include <cmath>
#include <limits>

#include "io/raw_input_types.h"

namespace lss {

static constexpr double kMinValue = std::numeric_limits<double>::lowest();

Batch::Batch(io::Batch raw_batch) :
    raw_batch_(std::move(raw_batch)), time_to_finish_(0) { }

bool Batch::operator<(const Batch& rhs) const {
  std::time_t now = std::time(nullptr);
  return Evaluate(now) < rhs.Evaluate(now);
}

bool Batch::operator==(const Batch& rhs) const {
  return raw_batch_ == rhs.raw_batch_ &&
      time_to_finish_ == rhs.time_to_finish_ &&
      jobs_ == rhs.jobs_;
}

double Batch::Evaluate(std::time_t time) const {
  if (!time_to_finish_)
    return kMinValue;
  return CurrentReward(time) / time_to_finish_;
}

double Batch::CurrentReward(double time) const {
  double r = (time - raw_batch_.due) / raw_batch_.expected_time;
  return raw_batch_.reward + raw_batch_.timely_reward / (1 + exp(r));
}

void Batch::AddJob(const io::Job& raw_job) {
  time_to_finish_ += raw_job.duration;
  jobs_.insert(raw_job);
}

int Batch::GetId() const {
  return raw_batch_.id;
}

std::set<io::Job, JobCmp> Batch::GetSortedJobs() const {
  return jobs_;
}

}  // namespace lss

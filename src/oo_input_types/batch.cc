#include "oo_input_types/batch.h"

#include <algorithm>
#include <ctime>
#include <cmath>
#include <limits>

namespace lss {

static constexpr double kMinValue = std::numeric_limits<double>::lowest();

Batch::Batch(const io::Batch& raw_batch) :
    raw_batch_(raw_batch), time_to_finish_(0) { }

bool Batch::operator<(const Batch& rhs) const {
  return Evaluate() < rhs.Evaluate();
}

bool Batch::operator==(const Batch& rhs) const {
  return raw_batch_.id == rhs.raw_batch_.id;
}

double Batch::Evaluate() const {
  if (!time_to_finish_)
    return kMinValue;
  std::time_t now = std::time(nullptr);
  double sigmoid_arg = (now - raw_batch_.due) / raw_batch_.expected_time;
  return Sigmoid(sigmoid_arg) / time_to_finish_;
}

double Batch::Sigmoid(double r) const {
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

#ifndef LSS_OO_INPUT_TYPES_BATCH_H_
#define LSS_OO_INPUT_TYPES_BATCH_H_

#include <set>
#include <ctime>
#include <cstdlib>
#include "io/raw_input_types.h"

namespace lss {

struct JobDurationCmp {
  bool operator()(const io::Job& job1, const io::Job& job2) const {
    return job1.duration < job2.duration;
  }
};

class Batch {
 public:
  explicit Batch(io::Batch raw_batch);

  bool operator==(const Batch& rhs) const;

  // Returns an evaluation of a batch at given time.
  // It is used to compare two batches by greedy scheduler
  // which favors batches with higher evaluation.
  // Batches without any jobs evaluate to -INF.
  double Evaluate(std::time_t time) const;
  // The reward that the objective function would give us
  // for completing the batch in moment 'time'
  double RewardAt(std::time_t time) const;
  void AddJob(const io::Job& raw_job);
  int GetId() const;

  // The returned reference is valid till a non-const method of Batch is called
  const std::set<io::Job, JobDurationCmp>& GetSortedJobs() const;

 private:
  io::Batch raw_batch_;
  double time_to_finish_;
  std::set<io::Job, JobDurationCmp> jobs_;
};

class BatchRewardCmp {
 public:
  BatchRewardCmp() : time_(std::time(nullptr)) {}

  bool operator() (Batch lhs, Batch rhs) {
    return lhs.Evaluate(time_) < rhs.Evaluate(time_);
  }

 private:
  std::time_t time_;
};

}  // namespace lss

#endif  // LSS_OO_INPUT_TYPES_BATCH_H_

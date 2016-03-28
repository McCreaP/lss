#ifndef LSS_GREEDY_BATCH_WRAPPER_H_
#define LSS_GREEDY_BATCH_WRAPPER_H_

#include <set>
#include <ctime>
#include <cstdlib>
#include "base/raw_situation.h"

namespace lss {
namespace greedy {

struct JobDurationCmp {
  bool operator()(const io::RawJob& job1, const io::RawJob& job2) const {
    return job1.duration < job2.duration;
  }
};

class BatchWrapper {
 public:
  explicit BatchWrapper(io::RawBatch raw_batch);

  bool operator==(const BatchWrapper& rhs) const;

  // Returns an evaluation of a batch at given time.
  // It is used to compare two batches by greedy scheduler
  // which favors batches with higher evaluation.
  // Batches without any jobs evaluate to -INF.
  double Evaluate(std::time_t time) const;
  // The reward that the objective function would give us
  // for completing the batch in moment 'time'
  double RewardAt(std::time_t time) const;
  void AddJob(const io::RawJob& raw_job);
  int GetId() const;

  // The returned reference is valid till a non-const method of BatchWrapper is called
  const std::set<io::RawJob, JobDurationCmp>& GetSortedJobs() const;

 private:
  io::RawBatch raw_batch_;
  double time_to_finish_;
  std::set<io::RawJob, JobDurationCmp> jobs_;
};

class BatchRewardCmp {
 public:
  BatchRewardCmp() : time_(std::time(nullptr)) {}

  bool operator() (BatchWrapper lhs, BatchWrapper rhs) {
    return lhs.Evaluate(time_) < rhs.Evaluate(time_);
  }

 private:
  std::time_t time_;
};

}  // namespace greedy
}  // namespace lss

#endif  // LSS_GREEDY_BATCH_WRAPPER_H_

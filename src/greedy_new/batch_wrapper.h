#ifndef LSS_GREEDY_NEW_BATCH_WRAPPER_H_
#define LSS_GREEDY_NEW_BATCH_WRAPPER_H_

#include <set>
#include <ctime>
#include <cstdlib>

#include "base/raw_situation.h"
#include "base/situation.h"

namespace lss {
namespace greedy_new {

struct JobDurationCmp {
  bool operator()(Job job1, Job job2) const {
    return job1.duration() < job2.duration();
  }
};

class BatchWrapper {
 public:
  explicit BatchWrapper(Batch batch);

  // Returns an evaluation of a batch at given time.
  // It is used to compare two batches by greedy scheduler
  // which favors batches with higher evaluation.
  // Batches without any jobs evaluate to -INF.
  double Evaluate(std::time_t time) const;
  // The reward that the objective function would give us
  // for completing the batch in moment 'time'
  double RewardAt(std::time_t time) const;

  const std::set<Job, JobDurationCmp>& GetSortedJobs() const;

 private:
  Batch batch_;
  double time_to_finish_;
  std::set<Job, JobDurationCmp> jobs_;
};

class BatchRewardCmp {
 public:
  BatchRewardCmp() : time_(std::time(nullptr)) { }

  bool operator()(BatchWrapper lhs, BatchWrapper rhs) {
    return lhs.Evaluate(time_) < rhs.Evaluate(time_);
  }

 private:
  std::time_t time_;
};

}  // namespace greedy_new
}  // namespace lss

#endif  // LSS_GREEDY_NEW_BATCH_WRAPPER_H_

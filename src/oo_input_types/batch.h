#ifndef LSS_OO_INPUT_TYPES_BATCH_H_
#define LSS_OO_INPUT_TYPES_BATCH_H_

#include <set>
#include <ctime>

#include "io/raw_input_types.h"

namespace lss {

struct JobCmp {
  bool operator()(const io::Job& job1, const io::Job& job2) {
    return job1.duration < job2.duration;
  }
};

class Batch {
 public:
  explicit Batch(io::Batch raw_batch);

  bool operator<(const Batch& rhs) const;
  bool operator==(const Batch& rhs) const;

  double Evaluate(std::time_t now) const;
  double CurrentReward(double r) const;
  void AddJob(const io::Job& raw_job);
  int GetId() const;

  std::set<io::Job, JobCmp> GetSortedJobs() const;

 private:
  io::Batch raw_batch_;
  double time_to_finish_;
  std::set<io::Job, JobCmp> jobs_;
};

}  // namespace lss

#endif  // LSS_OO_INPUT_TYPES_BATCH_H_

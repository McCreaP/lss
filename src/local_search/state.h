#ifndef LSS_LOCAL_SEARCH_STATE_H_
#define LSS_LOCAL_SEARCH_STATE_H_

#include <cstddef>
#include <limits>
#include <unordered_map>
#include <vector>

#include "base/situation.h"
#include "base/schedule.h"

namespace lss {
namespace local_search {

class State {
 public:
  State() = default;
  State(const State &) = default;
  State& operator=(const State&) = default;

  explicit State(Situation s);

  double Evaluate() const { return eval_; }
  Schedule ToSchedule() const;

  Machine GetMachine(Job j) const;
  size_t GetPos(Job j) const;

  size_t QueueSize(Machine m) const;
  Job QueueBack(Machine m) const;

  // Adds `j` at the end of job queue of `m`.
  void Assign(Machine m, Job j) { Assign(m, j, std::numeric_limits<size_t>::max()); }

  // Adds `j` to the Job queue of `m` at position `pos`. If `pos` exceeds the size of queue,
  // the job will be placed at its end.
  void Assign(Machine m, Job j, size_t pos);

 private:
  struct Entry {
    Entry() {}
    Entry(Job job) : job(job) {}
    Job job;
    Time finish_time{};
    double eval{};
  };
  using MachineQueue = std::vector<Entry>;

  double JobEval(Job j, Time start_time);
  void RecomputeTail(Machine m, size_t pos);
  MachineQueue::iterator FindJob(MachineQueue *queue, Job j);

  Situation situation_;
  double eval_;
  std::unordered_map<Machine, MachineQueue> queue_;
  std::unordered_map<Job, Machine> job_machine_;
};

}  // namespace local_search
}  // namespace lss

#endif  // LSS_LOCAL_SEARCH_STATE_H_

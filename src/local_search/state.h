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

// Where complexities are given M stands for the number of machines, B - batches, J - jobs
// in `situation` passed to constructor. All methods throw `std::invalid_argument` if an object
// is passed which is not present in `situation`. Null `Machine` is considered valid
// and represents the queue of unassigned jobs.
class State {
 public:
  State() = default;
  State(const State &) = default;
  State& operator=(const State&) = default;

  // Minimum requirements for for `situation`: all jobs must have a batch set
  // and all batches must have an account set. Otherwise `std::invalid_argument`
  // will be thrown.
  // Complexity: O(M + J + B).
  explicit State(Situation situation);

  // Returns an approximation of objective function for schedule represented by this `State`.
  // Complexity: O(1).
  double Evaluate() const { return eval_; }

  // Complexity: O(M + J).
  Schedule ToSchedule() const;

  // Returns null `Machine` if the job is not assigned.
  // Average complexity: O(1).
  Machine GetMachine(Job j) const;

  // Complexity: O(QueueSize(GetMachine(j)).
  size_t GetPos(Job j) const;

  // Complexity: O(1).
  size_t QueueSize(Machine m) const;

  // Complexity: O(1).
  Job QueueBack(Machine m) const;

  // Adds `j` at the end of job queue of `m`.
  // Complexity: O(QueueSize(GetMachine(j))).
  void Assign(Machine m, Job j) { Assign(m, j, std::numeric_limits<size_t>::max()); }

  // Adds `j` to the Job queue of `m` at position `pos`. If `pos` exceeds the size of queue,
  // the job will be placed at its end.
  // Complexity: O(QueueSize(GetMachine(j)) + QueueSize(m)).
  void Assign(Machine new_machine, Job job, size_t new_pos);

 private:
  struct Entry {
    Entry() {}
    explicit Entry(Job job) : job(job) {}

    Job job;
    Time finish_time{};
    double eval_contribution{};  // The contribution of `job` to global `eval_`.
  };
  using MachineQueue = std::vector<Entry>;

  double JobEval(Job j, Time finish_time) const;
  Time EstimatedStartTime(const MachineQueue &queue, size_t pos) const;
  void RecomputeTail(MachineQueue *queue, size_t pos);

  // Performs a linear search starting at the end. This will be significantly faster than
  // starting at the beginning for typical use of `State`.
  size_t JobPos(const MachineQueue &queue, Job j) const;

  Situation situation_;
  double eval_;
  // Queue of unassigned jobs is represented by a null Machine.
  std::unordered_map<Machine, MachineQueue> queue_;
  std::unordered_map<Job, Machine> assignment_;
};

}  // namespace local_search
}  // namespace lss

#endif  // LSS_LOCAL_SEARCH_STATE_H_

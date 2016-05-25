#ifndef LSS_BASE_SCHEDULE_H_
#define LSS_BASE_SCHEDULE_H_

#include <map>
#include <unordered_map>
#include <vector>
#include <bits/unordered_set.h>

#include "base/situation.h"
#include "io/basic_output.h"

namespace lss {

// Simple interface needed by genetic algorithm.
// Can be extended in the future.
class Schedule {
 public:
  using Jobs = std::vector<Job>;
  using Assignments = std::unordered_map<Machine, Jobs>;

  Schedule() = default;

  explicit Schedule(Situation situation) {
    for (Machine m : situation.machines()) {
      schedule_[m] = {};
    }
  }

  // Does not check whether machine is valid
  // or job can be executed on a given machine.
  void AssignJob(Machine machine, Job job) {
    schedule_[machine].push_back(job);
  }

  const Assignments &GetAssignments() const {
    return schedule_;
  };

 private:
  Assignments schedule_;
};

double ObjectiveFunction(const Schedule &schedule, Situation situation);

}  // namespace lss

#endif  // LSS_BASE_SCHEDULE_H_

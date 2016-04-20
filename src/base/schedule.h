#ifndef LSS_BASE_SCHEDULE_MOCK_H_
#define LSS_BASE_SCHEDULE_MOCK_H_

#include <map>
#include <unordered_map>
#include "situation.h"

namespace lss {

// Simple interface needed by genetic algorithm.
// Can be extended in the future.
class Schedule {
 public:
  using Jobs = std::vector<Job>;

  Schedule() = default;

  Schedule(const Situation &situation) {
    for (Machine m : situation.machines()) {
      schedule_[m] = {};
    }
  }

  // Does not check whether machine is valid
  // or job can be executed on a given machine.
  void AssignJob(Job job, Machine machine) {
    schedule_[machine].push_back(job);
  }

  const Jobs &GetJobsAssignedToMachine(Machine machine) const {
    return schedule_.at(machine);
  }

 private:
  std::unordered_map<Machine, Jobs> schedule_;
};

double ObjectiveFunction(const Schedule &schedule, const Situation &situation);

}  // namespace lss

#endif  // LSS_BASE_SCHEDULE_MOCK_H_

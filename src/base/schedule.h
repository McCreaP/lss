#ifndef LSS_BASE_SCHEDULE_H_
#define LSS_BASE_SCHEDULE_H_

#include <map>
#include <unordered_map>
#include <vector>

#include "base/situation.h"
#include "io/basic_output.h"

namespace lss {

// Simple interface needed by genetic algorithm.
// Can be extended in the future.
class Schedule {
 public:
  using Jobs = std::vector<Job>;

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

  const Jobs &GetJobsAssignedToMachine(Machine machine) const {
    return schedule_.at(machine);
  }

 private:
  std::unordered_map<Machine, Jobs> schedule_;
};

class AssignmentsHandler {
 public:
  explicit AssignmentsHandler(io::Writer *writer) : writer_(writer) { }

  void AdjustAssignments(const Schedule &schedule, Situation situation);

 private:
  enum class JobAssignmentState {
    kUnassigned = 0,
    kTaken = 1
  };

  using JobsAssignments = std::unordered_map<Id<Job>, Id<Machine>>;
  using MachinesAssignments = std::unordered_map<Id<Machine>, Id<Job>>;
  using JobsStates = std::unordered_map<Id<Job>, JobAssignmentState>;

  void RemoveNotPresentMachines(Situation situation);
  void RemoveNotPresentJobs(Situation situation);
  Job FindJobToAssign(const Schedule &schedule, Machine machine);
  bool CanBeAssigned(Job job);
  bool TryAssign(Machine machine, Job job);
  bool TryUnassign(MachinesAssignments::iterator assignment);

  io::Writer *writer_;
  JobsAssignments jobs_assignments_;
  MachinesAssignments machines_assignments_;
  JobsStates jobs_states_;
};

double ObjectiveFunction(const Schedule &schedule, Situation situation);

}  // namespace lss

#endif  // LSS_BASE_SCHEDULE_H_

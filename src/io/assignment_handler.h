#ifndef LSS_IO_ASSIGNMEN_THANDLER_H_
#define LSS_IO_ASSIGNMEN_THANDLER_H_

#include "base/schedule.h"
#include "io/basic_output.h"

namespace lss {
namespace io {

class AssignmentsHandler {
 public:
  explicit AssignmentsHandler(Writer *writer) : writer_(writer) { }

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
  Job FindJobToAssign(const Schedule::Jobs &jobs);
  bool CanBeAssigned(Job job);
  bool TryAssign(Machine machine, Job job);
  bool TryUnassign(MachinesAssignments::iterator assignment);

  Writer *writer_;
  JobsAssignments jobs_assignments_;
  MachinesAssignments machines_assignments_;
  JobsStates jobs_states_;
};

}  // namespace io
}  // namespace lss

#endif  // LSS_IO_ASSIGNMEN_THANDLER_H_

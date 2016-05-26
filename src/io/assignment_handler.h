#ifndef LSS_IO_ASSIGNMENT_HANDLER_H_
#define LSS_IO_ASSIGNMENT_HANDLER_H_

#include "base/schedule.h"
#include "io/basic_output.h"

namespace lss {
namespace io {

class AssignmentsState {
 public:
  using JobsAssignments = std::unordered_map<Id<Job>, Id<Machine>>;
  using MachinesAssignments = std::unordered_map<Id<Machine>, Id<Job>>;

  explicit AssignmentsState(Writer *writer) : writer_(writer) { }

  void RemoveNotPresentMachines(Situation situation);
  void RemoveNotPresentJobs(Situation situation);
  bool TryAssign(Id<Machine> machine_id, Id<Job> job_id);
  bool TryUnassign(Id<Machine> machine_id);
  Id<Machine> GetAssignedMachineId(Id<Job> job_id);
  bool HasBeenTaken(Id<Job> job_id);

 private:
  Writer *writer_;
  JobsAssignments jobs_assignments_;
  MachinesAssignments machines_assignments_;
  std::unordered_set<Id<Job>> taken_jobs_;
};

class AssignmentsHandler {
 public:
  explicit AssignmentsHandler(Writer *writer) : assignments_state_(writer) { }

  void AdjustAssignments(const Schedule &schedule, Situation situation);

 private:

  Job FindJobToAssign(const Schedule::Jobs &jobs);
  bool CanBeAssigned(Job job);

  AssignmentsState assignments_state_;
};

}  // namespace io
}  // namespace lss

#endif  // LSS_IO_ASSIGNMENT_HANDLER_H_

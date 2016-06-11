#ifndef LSS_IO_ASSIGNMENT_HANDLER_H_
#define LSS_IO_ASSIGNMENT_HANDLER_H_

#include "base/schedule.h"
#include "io/basic_output.h"

namespace lss {
namespace io {

enum class JobState {
  kNotAssigned = 0,
  kAssigned = 1,
  kTaken = 2
};

class AssignmentsState {
 public:
  using JobsAssignments = std::unordered_map<Id<Job>, Id<Machine>>;
  using MachinesAssignments = std::unordered_map<Id<Machine>, Id<Job>>;
  using MachinesContexts = std::unordered_map<Id<Machine>, Context>;

  explicit AssignmentsState(Writer *writer) : writer_(writer) { }

  void UpdateTakenJobs();
  void RemoveNotPresentMachines(const RawSituation &raw_situation);
  void RemoveNotPresentJobs(const RawSituation &raw_situation);
  bool TryAssign(Id<Machine> machine_id, Job job);
  bool TryUnassign(Id<Machine> machine_id);
  Id<Machine> GetAssignedMachineId(Id<Job> job_id);
  JobState KnownJobState(Id<Job> job_id);
  bool HasTakenAJob(Id<Machine> machine_id);
  void SwitchContext(Id<Machine> machine_id);
  Context GetMachineContext(Id<Machine> machine_id) const;

 private:
  Writer *writer_;
  JobsAssignments jobs_assignments_;
  MachinesAssignments machines_assignments_;
  std::unordered_set<Id<Job>> taken_jobs_;
  MachinesContexts machines_contexts_, machines_next_contexts_;
};

class AssignmentsHandler {
 public:
  explicit AssignmentsHandler(Writer *writer) : assignments_state_(writer) { }

  void AdjustRawSituation(RawSituation *raw_situation);
  void AdjustAssignments(const Schedule &schedule);

 private:
  void RemoveAlreadyTakenJobs(RawSituation *raw_situation);
  void FillMachineContexts(RawSituation *raw_situation);
  Job FindJobToAssign(const Schedule::Jobs &jobs);
  bool CanBeAssigned(Job job);

  AssignmentsState assignments_state_;
};

}  // namespace io
}  // namespace lss

#endif  // LSS_IO_ASSIGNMENT_HANDLER_H_

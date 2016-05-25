#include "io/assignment_handler.h"

namespace lss {
namespace io {

void AssignmentsHandler::AdjustAssignments(const Schedule &schedule, Situation situation) {
  RemoveNotPresentMachines(situation);
  RemoveNotPresentJobs(situation);
  for (const auto &assignment : schedule.GetAssignments()) {
    Machine machine = assignment.first;
    auto jobs = assignment.second;
    Job job_to_assign = FindJobToAssign(jobs);
    if (job_to_assign) {
      auto pending_assignment = machines_assignments_.find(machine.id());
      if (pending_assignment != machines_assignments_.end()) {
        TryUnassign(pending_assignment);
      }
      TryAssign(machine, job_to_assign);
    }
  }
}

void AssignmentsHandler::RemoveNotPresentMachines(Situation situation) {
  std::unordered_set<Id<Machine>> machines;
  for (Machine machine : situation.machines()) {
    machines.insert(machine.id());
  }
  auto machine_assignment = machines_assignments_.begin();
  while (machine_assignment != machines_assignments_.end()) {
    if (machines.count(machine_assignment->first) == 0) {
      TryUnassign(machine_assignment++);
    } else {
      ++machine_assignment;
    }
  }
}

void AssignmentsHandler::RemoveNotPresentJobs(Situation situation) {
  std::unordered_set<Id<Job>> jobs;
  for (Job job : situation.jobs()) {
    jobs.insert(job.id());
  }
  auto job_state = jobs_states_.begin();
  while (job_state != jobs_states_.end()) {
    if (jobs.count(job_state->first) == 0) {
      jobs_states_.erase(job_state++);
    } else {
      ++job_state;
    }
  }
}

Job AssignmentsHandler::FindJobToAssign(const Schedule::Jobs &jobs) {
  Job job_to_assign;
  for (Job job : jobs) {
    if (CanBeAssigned(job)) {
      job_to_assign = job;
      break;
    }
  }
  return job_to_assign;
}


bool AssignmentsHandler::CanBeAssigned(Job job) {
  auto jm = jobs_assignments_.find(job.id());
  if (jm != jobs_assignments_.end()) {
    auto assignment = machines_assignments_.find(jm->second);
    if (assignment != machines_assignments_.end()) {
      return TryUnassign(assignment);
    }
  }

  auto job_state = jobs_states_.find(job.id());
  if (job_state == jobs_states_.end()) {
    return true;
  }

  return job_state->second == JobAssignmentState::kUnassigned;
}

bool AssignmentsHandler::TryUnassign(MachinesAssignments::iterator assignment) {
  Id<Machine> machine_id = assignment->first;
  Id<Job> job_id = assignment->second;

  machines_assignments_.erase(assignment);
  jobs_assignments_.erase(job_id);
  bool successful_unassigned = writer_->Unassign(static_cast<IdType>(machine_id));
  jobs_states_[job_id] =
      successful_unassigned ? JobAssignmentState::kUnassigned : JobAssignmentState::kTaken;
  return successful_unassigned;
}

bool AssignmentsHandler::TryAssign(Machine machine, Job job) {
  IdType machine_id = static_cast<IdType>(machine.id());
  IdType job_id = static_cast<IdType>(job.id());
  if (writer_->Assign(machine_id, job_id)) {
    machines_assignments_.insert(std::make_pair(machine.id(), job.id()));
    jobs_assignments_.insert(std::make_pair(job.id(), machine.id()));
    jobs_states_.erase(job.id());
    return true;
  }
  return false;
}

}  // namespace io
}  // namespace lss
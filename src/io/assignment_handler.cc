#include "io/assignment_handler.h"

namespace lss {
namespace io {

void AssignmentsHandler::AdjustAssignments(const Schedule &schedule, Situation situation) {
  assignments_state_.RemoveNotPresentJobs(situation);
  assignments_state_.RemoveNotPresentMachines(situation);
  for (const auto &assignment : schedule.GetAssignments()) {
    Machine machine = assignment.first;
    auto jobs = assignment.second;
    Job job_to_assign = FindJobToAssign(jobs);
    if (job_to_assign) {
      assignments_state_.TryUnassign(machine.id());
      assignments_state_.TryAssign(machine.id(), job_to_assign.id());
    }
  }
}


void AssignmentsState::RemoveNotPresentJobs(Situation situation) {
  std::unordered_set<Id<Job>> jobs;
  for (Job job : situation.jobs()) {
    jobs.insert(job.id());
  }
  auto job_state = taken_jobs_.begin();
  while (job_state != taken_jobs_.end()) {
    if (jobs.count(*job_state) == 0) {
      taken_jobs_.erase(job_state++);
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
  Id<Machine> machine_id = assignments_state_.GetAssignedMachineId(job.id());
  if (machine_id) {
    return assignments_state_.TryUnassign(machine_id);
  }
  return !assignments_state_.HasBeenTaken(job.id());
}

void AssignmentsState::RemoveNotPresentMachines(Situation situation) {
  std::unordered_set<Id<Machine>> machines;
  for (Machine machine : situation.machines()) {
    machines.insert(machine.id());
  }
  auto machine_assignment = machines_assignments_.begin();
  while (machine_assignment != machines_assignments_.end()) {
    if (machines.count(machine_assignment->first) == 0) {
      TryUnassign((machine_assignment++)->first);
    } else {
      ++machine_assignment;
    }
  }
}

bool AssignmentsState::TryUnassign(Id<Machine> machine_id) {
  auto pending_assignment = machines_assignments_.find(machine_id);
  if (pending_assignment == machines_assignments_.end()) {
    return false;
  }
  Id<Job> job_id = pending_assignment->second;

  machines_assignments_.erase(pending_assignment);
  jobs_assignments_.erase(job_id);
  bool successful_unassigned = writer_->Unassign(static_cast<IdType>(machine_id));
  if (!successful_unassigned) {
    taken_jobs_.insert(job_id);
  }
  return successful_unassigned;
}

Id<Machine> AssignmentsState::GetAssignedMachineId(Id<Job> job_id) {
  auto job_assignment = jobs_assignments_.find(job_id);
  return job_assignment != jobs_assignments_.end() ? job_assignment->second : Id<Machine>();
}

bool AssignmentsState::HasBeenTaken(Id<Job> job_id) {
  return taken_jobs_.find(job_id) != taken_jobs_.end();
}

bool AssignmentsState::TryAssign(Id<Machine> machine_id, Id<Job> job_id) {
  if (writer_->Assign(static_cast<IdType>(machine_id), static_cast<IdType>(job_id))) {
    machines_assignments_.insert(std::make_pair(machine_id, job_id));
    jobs_assignments_.insert(std::make_pair(job_id, machine_id));
    return true;
  }
  return false;
}

}  // namespace io
}  // namespace lss

#include "glog/logging.h"

#include "io/assignment_handler.h"

namespace lss {
namespace io {

void AssignmentsHandler::AdjustRawSituation(RawSituation *raw_situation) {
  assignments_state_.UpdateTakenJobs();
  assignments_state_.RemoveNotPresentJobs(*raw_situation);
  assignments_state_.RemoveNotPresentMachines(*raw_situation);
  RemoveAlreadyTakenJobs(raw_situation);
  FillMachineContexts(raw_situation);
}

void AssignmentsHandler::RemoveAlreadyTakenJobs(RawSituation *raw_situation) {
  std::vector<RawJob> filtered_raw_jobs;
  for (const RawJob &raw_job : raw_situation->jobs_) {
    if (assignments_state_.KnownJobState(Id<Job>(raw_job.id_)) == JobState::kTaken) {
      continue;
    }
    filtered_raw_jobs.push_back(raw_job);
  }
  raw_situation->jobs_ = filtered_raw_jobs;
}

void AssignmentsHandler::FillMachineContexts(RawSituation *raw_situation) {
  for (RawMachine &raw_machine : raw_situation->machines_) {
    Context context = assignments_state_.GetMachineContext(Id<Machine>(raw_machine.id_));
    raw_machine.context(context);
  }
}

void AssignmentsHandler::AdjustAssignments(const Schedule &schedule) {
  for (const auto &assignment : schedule.GetAssignments()) {
    Machine machine = assignment.first;
    auto jobs = assignment.second;
    Job job_to_assign = FindJobToAssign(jobs);
    if (job_to_assign) {
      assignments_state_.TryUnassign(machine.id());
      assignments_state_.TryAssign(machine.id(), job_to_assign);
    }
  }
}

void AssignmentsState::UpdateTakenJobs() {
  // HasTakenAJob() can remove assignment from machines_assignments
  // so simple range for loop is not sufficient
  auto machine_assignment = machines_assignments_.begin();
  while (machine_assignment != machines_assignments_.end()) {
    HasTakenAJob((machine_assignment++)->first);
  }
}

void AssignmentsState::RemoveNotPresentJobs(const RawSituation &raw_situation) {
  std::unordered_set<Id<Job>> jobs;
  for (const RawJob &raw_job : raw_situation.jobs_) {
    jobs.insert(Id<Job>(raw_job.id_));
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

void AssignmentsState::RemoveNotPresentMachines(const RawSituation &raw_situation) {
  std::unordered_set<Id<Machine>> machines;
  for (const RawMachine &raw_machine : raw_situation.machines_) {
    machines.insert(Id<Machine>(raw_machine.id_));
  }
  auto machine_assignment = machines_assignments_.begin();
  while (machine_assignment != machines_assignments_.end()) {
    Id<Machine> machine_id = machine_assignment->first;
    if (machines.count(machine_id) == 0) {
      TryUnassign((machine_assignment++)->first);
      machines_contexts_.erase(machine_id);
      machines_next_contexts_.erase(machine_id);
    } else {
      ++machine_assignment;
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
  return assignments_state_.KnownJobState(job.id()) != JobState::kTaken;
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
    SwitchContext(machine_id);
  }
  return successful_unassigned;
}

Id<Machine> AssignmentsState::GetAssignedMachineId(Id<Job> job_id) {
  auto job_assignment = jobs_assignments_.find(job_id);
  return job_assignment != jobs_assignments_.end() ? job_assignment->second : Id<Machine>();
}

JobState AssignmentsState::KnownJobState(Id<Job> job_id) {
  if (jobs_assignments_.count(job_id)) {
    return JobState::kAssigned;
  } else if (taken_jobs_.count(job_id)) {
    return JobState::kTaken;
  } else {
    return JobState::kNotAssigned;
  }
}

bool AssignmentsState::TryAssign(Id<Machine> machine_id, Job job) {
  if (writer_->Assign(static_cast<IdType>(machine_id), static_cast<IdType>(job.id()))) {
    machines_assignments_.insert(std::make_pair(machine_id, job.id()));
    jobs_assignments_.insert(std::make_pair(job.id(), machine_id));
    machines_next_contexts_[machine_id] = job.context();
    return true;
  }
  return false;
}

bool AssignmentsState::HasTakenAJob(Id<Machine> machine_id) {
  auto machine_assignment = machines_assignments_.find(machine_id);
  Id<Job> job_id = machine_assignment->second;
  if (!writer_->DoesAssignmentExist(static_cast<IdType>(machine_id))) {
    machines_assignments_.erase(machine_id);
    jobs_assignments_.erase(job_id);
    taken_jobs_.insert(job_id);
    SwitchContext(machine_id);
    return true;
  }
  return false;
}

void AssignmentsState::SwitchContext(Id<Machine> machine_id) {
  auto machine_next_context = machines_next_contexts_.find(machine_id);
  machines_contexts_[machine_id] = machine_next_context->second;
  machines_next_contexts_.erase(machine_next_context);
}

Context AssignmentsState::GetMachineContext(Id<Machine> machine_id) const {
  auto context = machines_contexts_.find(machine_id);
  return context == machines_contexts_.end() ? Context() : context->second;
}

}  // namespace io
}  // namespace lss

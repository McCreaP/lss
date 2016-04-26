#include <cmath>
#include <set>
#include <unordered_set>

#include "base/schedule.h"
#include "base/situation.h"

namespace lss {
namespace {

using JobFinishTime = std::unordered_map<Job, Time>;

double Sigmoid(double reward, double timely_reward, Time time) {
  return reward + timely_reward / (1 + exp(time));
}

Time NormalizeTime(Batch batch, Time time) {
  return (time - batch.due()) / batch.duration();
}

double JobReward(Job job, Time time) {
  Batch batch = job.batch();
  return Sigmoid(batch.job_reward(), batch.job_timely_reward(), NormalizeTime(batch, time));
}

double BatchReward(Batch batch, Time time) {
  return Sigmoid(batch.reward(), batch.timely_reward(), NormalizeTime(batch, time));
}

double ChangeCost(const Situation &situation, Job from, Job to) {
  return situation.change_costs().cost(from.context(), to.context());
}

double JobsIngredient(const Schedule &schedule,
                      Situation situation,
                      JobFinishTime *job_finish_time) {
  double result = 0.;
  for (Machine machine : situation.machines()) {
    Time time = situation.time_stamp();
    auto jobs = schedule.GetJobsAssignedToMachine(machine);
    for (size_t i = 0; i < jobs.size(); ++i) {
      Job job = jobs[i];
      double change_cost = (i ? ChangeCost(situation, jobs[i - 1], job) : 0);
      time += job.duration() + change_cost;
      (*job_finish_time)[job] = time;
      result += JobReward(job, time);
    }
  }
  return result;
}

double BatchIngredient(Situation situation, const JobFinishTime &job_finish_time) {
  double result = 0.;
  for (Batch batch : situation.batches()) {
    Time batch_finish_time = std::numeric_limits<Time>::min();
    for (Job job : batch.jobs()) {
      if (job_finish_time.count(job)) {
        batch_finish_time = std::max(batch_finish_time, job_finish_time.at(job));
      }
    }
    if (batch_finish_time != std::numeric_limits<int>::min()) {
      result += BatchReward(batch, batch_finish_time);
    }
  }
  return result;
}

}  // namespace

void AssignmentsHandler::AdjustAssignments(const Schedule &schedule, Situation situation) {
  RemoveNotPresentMachines(situation);
  RemoveNotPresentJobs(situation);
  for (auto machine : situation.machines()) {
    Job job_to_assign = FindJobToAssign(schedule, machine);
    if (job_to_assign) {
      auto pending_assignment = machines_assignments_.find(machine);
      if (pending_assignment != machines_assignments_.end()) {
        TryUnassign(pending_assignment);
      }
      TryAssign(machine, job_to_assign);
    }
  }
}

void AssignmentsHandler::RemoveNotPresentMachines(Situation situation) {
  std::unordered_set<Machine> machines(situation.machines().begin(), situation.machines().end());
  for (auto mj = machines_assignments_.begin(); mj != machines_assignments_.end(); ++mj) {
    if (machines.count(mj->first) == 0) {
      TryUnassign(mj);
    }
  }
}

void AssignmentsHandler::RemoveNotPresentJobs(Situation situation) {
  std::unordered_set<Job> jobs(situation.jobs().begin(), situation.jobs().end());
  for (auto job_state = jobs_states_.begin(); job_state != jobs_states_.end(); ++job_state) {
    if (jobs.count(job_state->first) == 0) {
      jobs_states_.erase(job_state);
    }
  }
}

Job AssignmentsHandler::FindJobToAssign(const Schedule &schedule, Machine machine) {
  Job job_to_assign;
  auto m_schedule = schedule.GetJobsAssignedToMachine(machine);
  for (Job job : m_schedule) {
    if (CanBeAssigned(job)) {
      job_to_assign = job;
      break;
    }
  }
  return job_to_assign;
}


bool AssignmentsHandler::CanBeAssigned(Job job) {
  auto assignment = machines_assignments_.find(jobs_assignments_[job]);
  if (assignment != machines_assignments_.end()) {
    return TryUnassign(assignment);
  }

  auto job_state = jobs_states_.find(job);
  if (job_state == jobs_states_.end()) {
    return true;
  }

  return job_state->second == JobAssignmentState::kUnassigned;
}

bool AssignmentsHandler::TryUnassign(MachinesAssignments::iterator assignment) {
  Machine machine = assignment->first;
  Job job = assignment->second;
  int machine_id = static_cast<int>(machine.id());

  machines_assignments_.erase(assignment);
  jobs_assignments_.erase(job);
  bool successful_unassigned = writer_->Unassign(machine_id);
  jobs_states_[job] =
      successful_unassigned ? JobAssignmentState::kUnassigned : JobAssignmentState::kTaken;
  return successful_unassigned;
}

bool AssignmentsHandler::TryAssign(Machine machine, Job job) {
  int machine_id = static_cast<int>(machine.id());
  int job_id = static_cast<int>(job.id());
  if (writer_->Assign(machine_id, job_id)) {
    machines_assignments_.insert(std::make_pair(machine, job));
    jobs_assignments_.insert(std::make_pair(job, machine));
    return true;
  }
  return false;
}

double ObjectiveFunction(const Schedule &schedule, Situation situation) {
  JobFinishTime job_finish_time;
  double result = JobsIngredient(schedule, situation, &job_finish_time);
  result += BatchIngredient(situation, job_finish_time);
  return result;
}

}  // namespace lss

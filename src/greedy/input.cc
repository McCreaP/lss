#include "greedy/input.h"

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "io/basic_input.h"

namespace lss {
namespace greedy {

Input::Input(std::unique_ptr<io::Reader> reader) :
  reader_(std::move(reader)), context_changes_(std::make_shared<ContextChanges>()) { }

bool Input::Update() {
  RawSituation raw_data;
  if (!reader_->Read(&raw_data))
    return false;

  context_changes_->SetContextChanges(raw_data.change_costs_);
  UpdateBatches(raw_data.batches_);
  UpdateJobs(raw_data.jobs_);
  UpdateMachines(raw_data.machines_);
  UpdateMachineSets(raw_data.machine_sets_);
  return true;
}

void Input::Assign(const RawJob &raw_job, MachineWrapper* machine) {
  machine->AssignJob(raw_job);
  assigned_jobs_ids_.insert(raw_job.id_);
}

bool Input::IsJobAssigned(int job_id) const {
  return assigned_jobs_ids_.find(job_id) != assigned_jobs_ids_.end();
}

void Input::UpdateBatches(const std::vector<RawBatch> &raw_batches) {
  batches_.clear();
  for (const RawBatch &raw_batch : raw_batches)
    batches_.insert(std::make_pair(raw_batch.id_, BatchWrapper(raw_batch)));
}

void Input::UpdateJobs(const std::vector<RawJob> &raw_jobs) {
  for (const RawJob &raw_job : raw_jobs)
    batches_.at(raw_job.batch_).AddJob(raw_job);

  // We do not want to 'assigned_jobs_ids_' just growing with a time.
  // If a job is not in the input anymore,
  // we do not need to keep it in the 'assigned_jobs_ids_' set.
  std::unordered_set<int> updated_assigned_jobs_ids;
  for (const RawJob &raw_job : raw_jobs)
    if (IsJobAssigned(raw_job.id_))
      updated_assigned_jobs_ids.insert(raw_job.id_);
  assigned_jobs_ids_ = std::move(updated_assigned_jobs_ids);
}

void Input::UpdateMachines(const std::vector<RawMachine> &raw_machines) {
  std::unordered_map<int, std::shared_ptr<MachineWrapper>> updated_machines;
  for (const RawMachine &raw_machine : raw_machines) {
    std::shared_ptr<MachineWrapper> machine;
    auto machines_iter = machines_.find(raw_machine.id_);
    if (machines_iter != machines_.end()) {
      machine = machines_iter->second;
      machine->SetState(raw_machine.state_);
    } else {
      machine = std::make_shared<MachineWrapper>(raw_machine, context_changes_);
    }
    updated_machines.insert(std::make_pair(machine->GetId(), machine));
  }
  machines_ = std::move(updated_machines);
}

void Input::UpdateMachineSets(const std::vector<RawMachineSet> &raw_machine_sets) {
  machines_from_set_.clear();
  for (const RawMachineSet &raw_machine_set : raw_machine_sets) {
    machines_from_set_[raw_machine_set.id_];  // Machine set can contain no machines
    for (int machine_id : raw_machine_set.machines_)
      machines_from_set_[raw_machine_set.id_].push_back(machines_[machine_id]);
  }
}

std::vector<BatchWrapper> Input::GetBatches() const {
  std::vector<BatchWrapper> batches;
  for (const auto &batch : batches_)
    batches.push_back(batch.second);
  return batches;
}

const std::vector<std::shared_ptr<MachineWrapper>> &Input::GetMachinesFromSet(int set_id) const {
  return machines_from_set_.at(set_id);
}

}  // namespace greedy
}  // namespace lss

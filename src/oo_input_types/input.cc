#include "oo_input_types/input.h"

#include <algorithm>
#include <unordered_map>
#include <memory>
#include <unordered_set>

#include "io/basic_input.h"

namespace lss {

Input::Input(std::shared_ptr<io::Reader> reader) : reader_(reader) { }

bool Input::Update() {
  io::RawData raw_data;
  if (!reader_->Read(&raw_data))
    return false;

  UpdateBatches(raw_data.batches);
  UpdateJobs(raw_data.jobs);
  UpdateMachines(raw_data.machines, raw_data.context_changes);
  UpdateMachineSets(raw_data.machine_sets);
  return true;
}

void Input::Assign(const io::Job& raw_job, Machine* machine) {
  machine->AssignJob(raw_job);
  assigned_jobs_ids_.insert(raw_job.id);
}

bool Input::IsJobAssigned(int job_id) const {
  return assigned_jobs_ids_.find(job_id) != assigned_jobs_ids_.end();
}

void Input::UpdateBatches(const std::vector<io::Batch>& raw_batches) {
  batches_.clear();
  for (const io::Batch& raw_batch : raw_batches)
    batches_.insert(std::make_pair(raw_batch.id, Batch(raw_batch)));
}

void Input::UpdateJobs(const std::vector<io::Job>& raw_jobs) {
  for (const io::Job& raw_job : raw_jobs)
    batches_.at(raw_job.batch_id).AddJob(raw_job);

  // We do not want to 'assigned_jobs_ids_' just growing with a time.
  // If a job is not in the input anymore,
  // we do not need to keep it in the 'assigned_jobs_ids_' set.
  std::unordered_set<int> updated_assigned_jobs_ids;
  for (const io::Job& raw_job : raw_jobs)
    if (IsJobAssigned(raw_job.id))
      updated_assigned_jobs_ids.insert(raw_job.id);
  assigned_jobs_ids_ = std::move(updated_assigned_jobs_ids);
}

void Input::UpdateMachines(const std::vector<io::Machine>& raw_machines,
                           const std::vector<io::ContextChange>& raw_context_changes) {
  Machine::SetContextChanges(raw_context_changes);

  std::unordered_map<int, std::shared_ptr<Machine>> updated_machines;
  for (const io::Machine& raw_machine : raw_machines) {
    std::shared_ptr<Machine> machine;
    auto machines_iter = machines_.find(raw_machine.id);
    if (machines_iter != machines_.end()) {
      machine = machines_iter->second;
      machine->SetState(raw_machine.state);
    } else {
      machine = std::make_shared<Machine>(raw_machine);
    }
    updated_machines.insert(std::make_pair(machine->GetId(), machine));
  }
  machines_ = std::move(updated_machines);
}

void Input::UpdateMachineSets(const std::vector<io::MachineSet>& raw_machine_sets) {
  machines_from_set_.clear();
  for (const io::MachineSet& raw_machine_set : raw_machine_sets) {
    machines_from_set_[raw_machine_set.id];  // Machine set can contain no machines
    for (int machine_id : raw_machine_set.machines)
      machines_from_set_[raw_machine_set.id].push_back(machines_[machine_id]);
  }
}

std::vector<Batch> Input::GetSortedBatches() const {
  std::vector<Batch> batches;
  for (const auto& batch : batches_)
    batches.push_back(batch.second);
  std::sort(std::begin(batches), std::end(batches));
  return batches;
}

std::vector<std::shared_ptr<Machine>> Input::GetMachinesFromSet(int set_id) const {
  return machines_from_set_.at(set_id);
}

}  // namespace lss

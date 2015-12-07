#include "input.h"

#include <algorithm>
#include <unordered_map>
#include <memory>
#include <set>

#include "io/basic_input.h"

namespace lss {

Input::Input(std::shared_ptr<io::Reader> reader) : reader_(reader) { }

bool Input::Update() {
  io::RawData raw_data;
  if (!reader_->Read(&raw_data))
    return false;

  UpdateBatches(raw_data.batches, raw_data.jobs);
  UpdateMachines(raw_data.machines, raw_data.context_changes);
  UpdateMachineSets(raw_data.machine_sets);
  return true;
}

void Input::UpdateBatches(const std::vector<io::Batch>& raw_batches,
                          const std::vector<io::Job>& raw_jobs) {
  batches_.clear();
  for (const io::Batch& raw_batch : raw_batches)
    batches_.insert(std::make_pair(raw_batch.id, Batch(raw_batch)));
  for (const io::Job& raw_job : raw_jobs)
    batches_.at(raw_job.batch_id).AddJob(raw_job);
}

void Input::UpdateMachines(const std::vector<io::Machine>& raw_machines,
                           const std::vector<io::ContextChange>& raw_context_changes) {
  Machine::SetContextChanges(raw_context_changes);

  std::unordered_map<int, std::shared_ptr<Machine>> updated_machines;
  for (const io::Machine& raw_machine : raw_machines) {
    std::shared_ptr<Machine> machine;
    auto machines_iter = machines_.find(raw_machine.id);
    if (machines_iter != machines_.end()) {
      machines_iter->second->UpdateState(raw_machine.state);
      machine = machines_iter->second;
    } else {
      machine = std::make_shared<Machine>(raw_machine);
    }
    updated_machines.insert(std::make_pair(machine->GetId(), machine));
  }
  machines_ = std::move(updated_machines);
}

void Input::UpdateMachineSets(const std::vector<io::MachineSet>& raw_machine_sets) {
  machines_from_set_.clear();
  for (const io::MachineSet& raw_machine_set : raw_machine_sets)
    for (const auto& machine : machines_)
      machines_from_set_[raw_machine_set.id].push_back(machine.second);
}

std::vector<Batch> Input::GetSortedBatches() const {
  std::vector<Batch> batches;
  for (auto batch : batches_)
    batches.push_back(batch.second);
  std::sort(std::begin(batches), std::end(batches));
  return batches;
}

std::vector<std::shared_ptr<Machine>> Input::GetMachinesFromSet(int set_id) const {
  return machines_from_set_.at(set_id);
}

}  // namespace lss

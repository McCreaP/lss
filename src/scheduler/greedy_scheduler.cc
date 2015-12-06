#include "scheduler/greedy_scheduler.h"

#include <iostream>

#include "io/basic_input.h"
#include "oo_input_types/batch.h"
#include "oo_input_types/machine.h"

namespace lss {

static constexpr double kMaxContextChangeCost = 1000 * 1000 * 1000;

GreedyScheduler::GreedyScheduler(const std::string& input_path)
    : input_(std::shared_ptr<io::Reader>(new io::BasicReader(input_path))),
      basic_writer_(input_path + "assignments/") { }

void GreedyScheduler::Schedule() {
  while (true) {
    if (!input_.Update())
      continue;
    auto sorted_batches = input_.GetSortedBatches();
    for (const Batch& batch : sorted_batches)
      AssignJobsFromBatch(batch);
  }
}


Machine* FindBestMachine(const io::Job& raw_job, const MachineSet& machine_set) {
  double min_context_changed_cost = kMaxContextChangeCost;
  Machine* best_machine = nullptr;
  for (Machine& machine : machine_set.GetIdleMachines()) {
    if (machine.ContextChangeCost(raw_job) < min_context_changed_cost) {
      best_machine = &machine;
      min_context_changed_cost = machine.ContextChangeCost(raw_job);
    }
  }
  return best_machine;
}

void GreedyScheduler::AssignJobsFromBatch(const Batch& batch) {
  for (const io::Job& job : batch.GetSortedJobs()) {
    auto machine_set = input_.GetMachineSet(job.machineset_id);
    Machine* best_machine = FindBestMachine(job, machine_set);
    if (best_machine) {
      best_machine->AssignJob(job);
      basic_writer_.Assign(best_machine->GetId(), job.id);
    }
  }
}


}  // namespace lss

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


int FindBestMachine(const io::Job& raw_job, const MachineSet& machine_set) {
  double min_context_changed_cost = kMaxContextChangeCost;
  int best_machine_id = -1;
  for (Machine& machine : machine_set.GetMachines()) {
    if (!machine.IsWaitingForJob())
      continue;
    if (machine.ContextChangeCost(raw_job) < min_context_changed_cost) {
      best_machine_id = machine.GetId();
      min_context_changed_cost = machine.ContextChangeCost(raw_job);
    }
  }
  return best_machine_id;
}

void GreedyScheduler::AssignJobsFromBatch(const Batch& batch) {
  for (const io::Job& job : batch.GetSortedJobs()) {
    auto machine_set = input_.GetMachineSet(job.machineset_id);
    int best_machine_id = FindBestMachine(job, machine_set);
    if (best_machine_id != -1) {
      input_.GetMachine(best_machine_id).AssignJob(job);
      basic_writer_.Assign(best_machine_id, job.id);
    }
  }
}


}  // namespace lss

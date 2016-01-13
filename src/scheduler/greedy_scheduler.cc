#include "scheduler/greedy_scheduler.h"

#include <iostream>
#include <memory>
#include <pstreams/pstream.h>
#include "io/basic_input.h"
#include "oo_input_types/batch.h"
#include "oo_input_types/machine.h"

namespace lss {

static constexpr double kMaxContextChangeCost = 1000 * 1000 * 1000;

GreedyScheduler::GreedyScheduler(const std::string& input_path)
    : input_(std::make_shared<io::BasicReader>(input_path)),
      basic_writer_(input_path + "assignments/") { }

void GreedyScheduler::Schedule() {
  while (true) {
    if (!input_.Update()) {
      redi::ipstream proc("curl localhost:8000 > /dev/null", redi::pstreams::pstderr);
      continue;
    }
    auto sorted_batches = input_.GetSortedBatches();
    for (auto batch = sorted_batches.crbegin(); batch != sorted_batches.crend(); ++batch)
      AssignJobsFromBatch(*batch);
  }
}

std::shared_ptr<Machine> GreedyScheduler::FindBestMachine(const io::Job& raw_job) {
  double min_context_changed_cost = kMaxContextChangeCost;
  auto machines = input_.GetMachinesFromSet(raw_job.machineset_id);
  std::shared_ptr<Machine> best_machine;
  for (std::shared_ptr<Machine> machine : machines) {
    if (!machine->IsWaitingForAJob())
      continue;
    double context_change_cost = machine->ContextChangeCost(raw_job);
    if (context_change_cost < min_context_changed_cost) {
      best_machine = machine;
      min_context_changed_cost = context_change_cost;
    }
  }
  return best_machine;
}

void GreedyScheduler::AssignJobsFromBatch(const Batch& batch) {
  for (const io::Job& job : batch.GetSortedJobs()) {
    if (input_.IsJobAssigned(job.id))
      continue;
    std::shared_ptr<Machine> best_machine = FindBestMachine(job);
    if (best_machine) {
      input_.Assign(best_machine, job);
      basic_writer_.Assign(best_machine->GetId(), job.id);
    }
  }
}

}  // namespace lss

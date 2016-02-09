#include "scheduler/greedy_scheduler.h"

#include <glog/logging.h>
#include <pstreams/pstream.h>

#include <algorithm>
#include <iostream>
#include <limits>
#include <memory>

#include "io/basic_input.h"
#include "oo_input_types/batch.h"
#include "oo_input_types/machine.h"

namespace lss {
namespace {

void NotifyDriverIFinishedCompute() {
  redi::ipstream proc("curl localhost:8000 > /dev/null", redi::pstreams::pstderr);
}

}  // namespace

static constexpr double kMaxContextChangeCost = std::numeric_limits<double>::infinity();

GreedyScheduler::GreedyScheduler(const std::string &input_path,
                                 const std::string &assignments_path)
  : input_(std::make_unique<io::BasicReader>(input_path)),
    basic_writer_(assignments_path) { }

void GreedyScheduler::Schedule() {
  while (true) {
    if (!input_.Update())
      continue;
    LOG(INFO) << "Read new input. Starting scheduling iteration";
    auto batches = input_.GetBatches();
    std::sort(std::begin(batches), std::end(batches), BatchRewardCmp());
    for (auto batch = batches.crbegin(); batch != batches.crend(); ++batch) {
      AssignJobsFromBatch(*batch);
    }
    LOG(INFO) << "Finished scheduling iteration";
    NotifyDriverIFinishedCompute();
  }
}

std::shared_ptr<Machine> GreedyScheduler::FindBestMachine(const io::Job &raw_job) {
  VLOG(1) << "FindBestMachine for Job: " << raw_job.id;
  double min_context_changed_cost = kMaxContextChangeCost;
  auto machines = input_.GetMachinesFromSet(raw_job.machineset_id);
  VLOG(1) << "Got " << machines.size() << " machines from machine_set";
  std::shared_ptr<Machine> best_machine;
  for (std::shared_ptr<Machine> machine : machines) {
    if (!machine->IsWaitingForAJob())
      VLOG(1) << "Trying machine: " << machine->GetId();
    if (!machine->IsWaitingForAJob()) {
      VLOG(1) << "Machine is not waiting for a job;
      continue;
    }
    double context_change_cost = machine->ContextChangeCost(raw_job);
    VLOG(1) << "Context change cost: " << context_change_cost;
    if (context_change_cost < min_context_changed_cost) {
      VLOG(1) << "Found new best machine: " << machine->GetId();
      best_machine = machine;
      min_context_changed_cost = context_change_cost;
    }
  }
  VLOG(1) << "Best machine: " << best_machine->GetId();
  return best_machine;
}

void GreedyScheduler::AssignJobsFromBatch(const Batch &batch) {
  VLOG(1) << "Assign jobs from batch: " << batch.GetId();
  for (const io::Job &job : batch.GetSortedJobs()) {
    VLOG(1) << "Job: " << job.id;
    if (input_.IsJobAssigned(job.id)) {
      VLOG(1) << "Already assigned";
      continue;
    }
    VLOG(1) << "Trying to assign job";
    std::shared_ptr<Machine> best_machine = FindBestMachine(job);
    if (best_machine) {
      VLOG(1) << "Best machine found: " << best_machine->GetId();
      if (basic_writer_.Assign(best_machine->GetId(), job.id)) {
        LOG(INFO) << "Assigning job succeed";
        input_.Assign(job, best_machine.get());
      } else {
        LOG(ERROR) << "Assigning job failed" << std::endl;
      }
    }
  }
}

}  // namespace lss

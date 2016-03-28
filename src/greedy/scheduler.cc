#include "greedy/scheduler.h"

#include <glog/logging.h>
#include <pstreams/pstream.h>

#include <algorithm>
#include <iostream>
#include <limits>
#include <memory>

#include "io/basic_input.h"
#include "greedy/batch_wrapper.h"
#include "greedy/machine_wrapper.h"

namespace lss {
namespace greedy {
namespace {

void NotifyDriverIFinishedCompute() {
  redi::ipstream proc("curl localhost:8000 > /dev/null", redi::pstreams::pstderr);
}

}  // namespace

static constexpr double kMaxContextChangeCost = std::numeric_limits<double>::infinity();

Scheduler::Scheduler(const std::string &input_path,
                                 const std::string &assignments_path)
  : input_(std::make_unique<io::BasicReader>(input_path)),
    basic_writer_(assignments_path) { }

void Scheduler::Schedule() {
  while (true) {
    if (!input_.Update())
      continue;
    VLOG(1) << "Read new input. Starting scheduling iteration";
    auto batches = input_.GetBatches();
    std::sort(std::begin(batches), std::end(batches), BatchRewardCmp());
    for (auto batch = batches.crbegin(); batch != batches.crend(); ++batch) {
      AssignJobsFromBatch(*batch);
    }
    VLOG(1) << "Finished scheduling iteration";
    NotifyDriverIFinishedCompute();
  }
}

void Scheduler::AssignJobsFromBatch(const BatchWrapper &batch) {
  VLOG(1) << "Assign jobs from batch: " << batch.GetId();
  for (const io::RawJob &job : batch.GetSortedJobs()) {
    VLOG(2) << "Job: " << job.id;
    if (input_.IsJobAssigned(job.id)) {
      VLOG(2) << "Already assigned";
      continue;
    }
    VLOG(2) << "Trying to assign job";
    std::shared_ptr<MachineWrapper> best_machine = FindBestMachine(job);
    if (best_machine) {
      VLOG(2) << "Best machine found: " << best_machine->GetId();
      if (basic_writer_.Assign(best_machine->GetId(), job.id)) {
        VLOG(2) << "Assigning job succeed";
        input_.Assign(job, best_machine.get());
      } else {
        LOG(ERROR) << "Assigning job failed" << std::endl;
      }
    }
  }
}

std::shared_ptr<MachineWrapper> Scheduler::FindBestMachine(const io::RawJob &raw_job) {
  VLOG(3) << "FindBestMachine for Job: " << raw_job.id;
  double min_context_changed_cost = kMaxContextChangeCost;
  auto machines = input_.GetMachinesFromSet(raw_job.machineset_id);
  VLOG(3) << "Got " << machines.size() << " machines from machine_set";
  std::shared_ptr<MachineWrapper> best_machine;
  for (std::shared_ptr<MachineWrapper> machine : machines) {
    VLOG(4) << "Trying machine: " << machine->GetId();
    if (!machine->IsWaitingForAJob()) {
      VLOG(4) << "MachineWrapper is not waiting for a job";
      continue;
    }
    double context_change_cost = machine->ContextChangeCost(raw_job);
    VLOG(4) << "Context change cost: " << context_change_cost;
    if (context_change_cost < min_context_changed_cost) {
      VLOG(4) << "Found new best machine: " << machine->GetId();
      best_machine = machine;
      min_context_changed_cost = context_change_cost;
    }
  }
  return best_machine;
}

}  // namespace greedy
}  // namespace lss

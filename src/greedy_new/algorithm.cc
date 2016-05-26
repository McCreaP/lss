#include <limits>
#include <algorithm>
#include <vector>

#include "glog/logging.h"

#include "greedy_new/algorithm.h"

namespace lss {
namespace greedy_new {

Schedule GreedyAlgorithm::Runner::Run() {
  std::vector<BatchWrapper> batches;
  for (Batch batch : situation_.batches()) {
    batches.push_back(BatchWrapper(batch));
  }
  std::sort(std::begin(batches), std::end(batches), BatchRewardCmp());
  for (auto batch = batches.crbegin(); batch != batches.crend(); ++batch) {
    AssignJobsFromBatch(*batch);
  }
  return schedule_;
}

void GreedyAlgorithm::Runner::AssignJobsFromBatch(const BatchWrapper &batch) {
  for (Job job : batch.GetSortedJobs()) {
    Machine best_machine = FindBestMachine(job);
    if (best_machine) {
      schedule_.AssignJob(best_machine, job);
      used_machines_.insert(best_machine);
    }
  }
}

Machine GreedyAlgorithm::Runner::FindBestMachine(Job job) const {
  double min_context_change_cost = std::numeric_limits<double>::max();
  Machine best_machine;
  for (Machine machine : job.machine_set().machines()) {
    if (used_machines_.count(machine)) {
      continue;
    }
    double change_cost = situation_.change_costs().cost(machine.context(), job.context());
    if (change_cost < min_context_change_cost) {
      best_machine = machine;
      min_context_change_cost = change_cost;
    }
  }
  return best_machine;
}

}  // namespace greedy_new
}  // namespace lss

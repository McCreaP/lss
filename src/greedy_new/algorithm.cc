#include <limits>
#include <algorithm>
#include <vector>

#include "glog/logging.h"

#include "greedy_new/algorithm.h"

namespace lss {
namespace greedy_new {

Schedule GreedyAlgorithm::Runner::Run() {
  for (Machine machine : situation_.machines()) {
    available_at_[machine];
    last_context_[machine] = machine.context();
  }
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
      last_context_[best_machine] = job.context();
      double change_cost = situation_.change_costs().cost(last_context_[best_machine], job.context());
      available_at_[best_machine] += change_cost + job.duration();
    }
  }
}

Machine GreedyAlgorithm::Runner::FindBestMachine(Job job) const {
  double min_start_time = std::numeric_limits<double>::max();
  Machine best_machine;
  for (Machine machine : job.machine_set().machines()) {
    double change_cost = situation_.change_costs().cost(last_context_.at(machine), job.context());
    if (available_at_.at(machine) + change_cost < min_start_time) {
      best_machine = machine;
      min_start_time = available_at_.at(machine) + change_cost;
    }
  }
  return best_machine;
}

}  // namespace greedy_new
}  // namespace lss

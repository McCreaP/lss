#ifndef LSS_GREEDY_NEW_ALGORITHM_H_
#define LSS_GREEDY_NEW_ALGORITHM_H_

#include <unordered_set>

#include "base/algorithm.h"
#include "greedy_new/batch_wrapper.h"

namespace lss {
namespace greedy_new {

class GreedyAlgorithm: public Algorithm {
 public:
  Schedule Run(__attribute__((unused)) const Schedule &prev_schedule,
               Situation new_situation) override {
    return GreedyAlgorithmRunner(new_situation).Run();
  }

 private:
  class GreedyAlgorithmRunner {
   public:
    explicit GreedyAlgorithmRunner(Situation situation) : situation_(situation) {}
    Schedule Run();

   private:
    void AssignJobsFromBatch(const BatchWrapper &batchWrapper);
    Machine FindBestMachine(Job job) const;

    Schedule schedule_;
    Situation situation_;
    std::unordered_set<Machine> used_machines_;
  };
};



}  // namespace greedy_new
}  // namespace lss

#endif  // LSS_GREEDY_NEW_ALGORITHM_H_

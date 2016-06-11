#ifndef LSS_GREEDY_NEW_ALGORITHM_H_
#define LSS_GREEDY_NEW_ALGORITHM_H_

#include <unordered_set>

#include "glog/logging.h"

#include "base/algorithm.h"
#include "greedy_new/batch_wrapper.h"

namespace lss {
namespace greedy_new {

class GreedyAlgorithm: public Algorithm {
 public:
  Schedule Run(__attribute__((unused)) const Schedule &prev_schedule,
               Situation new_situation) override {
    return Runner(new_situation).Run();
  }

 private:
  class Runner {
   public:
    explicit Runner(Situation situation) : situation_(situation) {}
    Schedule Run();

   private:
    void AssignJobsFromBatch(const BatchWrapper &batchWrapper);
    Machine FindBestMachine(Job job) const;

    Schedule schedule_;
    Situation situation_;
    std::unordered_map<Machine, Time> available_at_;
    std::unordered_map<Machine, Context> last_context_;
  };
};



}  // namespace greedy_new
}  // namespace lss

#endif  // LSS_GREEDY_NEW_ALGORITHM_H_

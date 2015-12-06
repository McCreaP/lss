#ifndef LSS_SCHEDULER_GREEDY_SCHEDULER_H_
#define LSS_SCHEDULER_GREEDY_SCHEDULER_H_

#include <string>

#include "io/basic_output.h"
#include "io/input.h"

namespace lss {

class GreedyScheduler {
 public:
  explicit GreedyScheduler(const std::string& input_path);

  void Schedule();

 private:
  void AssignJobsFromBatch(const Batch& batch);

  Input input_;
  io::BasicWriter basic_writer_;
};

}  // namespace lss

#endif  // LSS_SCHEDULER_GREEDY_SCHEDULER_H_

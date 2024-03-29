#ifndef LSS_GREEDY_SCHEDULER_H_
#define LSS_GREEDY_SCHEDULER_H_

#include <string>

#include "io/basic_output.h"
#include "greedy/input.h"

namespace lss {
namespace greedy {

class Scheduler {
 public:
  // 'input_path' should name a file (not directory) with input data.
  // 'assignments_path' should name a directory
  // with files representing assigning jobs to machines.
  explicit Scheduler(const std::string& input_path, const std::string& assignments_path);

  void Schedule();

 private:
  void AssignJobsFromBatch(const BatchWrapper& batch);
  std::shared_ptr<MachineWrapper> FindBestMachine(const RawJob& raw_job);

  Input input_;
  io::BasicWriter basic_writer_;
};

}  // namespace greedy
}  // namespace lss

#endif  // LSS_GREEDY_SCHEDULER_H_

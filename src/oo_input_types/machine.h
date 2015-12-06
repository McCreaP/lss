#ifndef LSS_OO_INPUT_TYPES_MACHINE_H_
#define LSS_OO_INPUT_TYPES_MACHINE_H_

#include <vector>

#include "io/raw_input_types.h"

namespace lss {

class Machine {
 public:
  static void SetContextChanges(const std::vector<io::ContextChange>& raw_changes);

  explicit Machine(const io::Machine& raw_machine);

  int GetId() const;
  void UpdateState(io::MachineState new_state);
  double ContextChangeCost(const io::Job& raw_job) const;
  void AssignJob(const io::Job& raw_job);
  bool IsIdle() const;

 private:
  static int context_changes_[2][2][2];

  io::Machine raw_machine_;
  int context_[3];
  bool job_assigned_;
};

}  // namespace lss

#endif  // LSS_OO_INPUT_TYPES_MACHINE_H_

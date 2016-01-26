#ifndef LSS_OO_INPUT_TYPES_MACHINE_H_
#define LSS_OO_INPUT_TYPES_MACHINE_H_

#include <memory>
#include <vector>

#include "io/raw_input_types.h"

namespace lss {

class ContextChanges {
 public:
  void SetContextChanges(const std::vector<io::ContextChange> raw_changes);
  int GetContextChangeCost(bool changed0, bool changed1, bool changed2) const;

 private:
  int context_changes_[2][2][2];
};

class Machine {
 public:
  explicit Machine(io::Machine raw_machine, std::shared_ptr<ContextChanges> context_changes);

  bool operator==(const Machine& rhs) const;
  int GetId() const;
  void SetState(io::MachineState new_state);
  double ContextChangeCost(const io::Job& raw_job) const;
  void AssignJob(const io::Job& raw_job);
  bool IsWaitingForAJob() const;

 private:
  io::Machine raw_machine_;
  int context_[3];
  std::shared_ptr<ContextChanges> context_changes_;
  bool has_assigned_job_;
};

}  // namespace lss

#endif  // LSS_OO_INPUT_TYPES_MACHINE_H_

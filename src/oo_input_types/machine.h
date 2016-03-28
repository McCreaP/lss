#ifndef LSS_OO_INPUT_TYPES_MACHINE_H_
#define LSS_OO_INPUT_TYPES_MACHINE_H_

#include <memory>
#include <vector>

#include "base/raw_situation.h"

namespace lss {

class ContextChanges {
 public:
  void SetContextChanges(const std::vector<io::RawContextChange> raw_changes);
  int GetContextChangeCost(bool changed0, bool changed1, bool changed2) const;

 private:
  int context_changes_[2][2][2];
};

class Machine {
 public:
  explicit Machine(io::RawMachine raw_machine, std::shared_ptr<ContextChanges> context_changes);

  bool operator==(const Machine& rhs) const;
  int GetId() const;
  void SetState(io::MachineState new_state);
  double ContextChangeCost(const io::RawJob& raw_job) const;
  void AssignJob(const io::RawJob& raw_job);
  // Now 'has_assigned_job_' is set back to false
  // when Input::Update() detects the machine has changed its state from kIdle.
  // It causes potentially a race condition - if the machine processes
  // an assigned job very quick Input::Update() won't notice it.
  bool IsWaitingForAJob() const;

 private:
  io::RawMachine raw_machine_;
  int context_[3];
  std::shared_ptr<ContextChanges> context_changes_;
  bool has_assigned_job_;
};

}  // namespace lss

#endif  // LSS_OO_INPUT_TYPES_MACHINE_H_

#ifndef LSS_GREEDY_MACHINE_WRAPPER_H_
#define LSS_GREEDY_MACHINE_WRAPPER_H_

#include <memory>
#include <vector>

#include "base/raw_situation.h"

namespace lss {
namespace greedy {

class ContextChanges {
 public:
  void SetContextChanges(const std::vector<RawChangeCost> raw_changes);
  int GetContextChangeCost(bool changed0, bool changed1, bool changed2) const;

 private:
  int context_changes_[2][2][2];
};

class MachineWrapper {
 public:
  explicit MachineWrapper(RawMachine raw_machine, std::shared_ptr<ContextChanges> context_changes);

  bool operator==(const MachineWrapper& rhs) const;
  int GetId() const;
  void SetState(MachineState new_state);
  double ContextChangeCost(const RawJob& raw_job) const;
  void AssignJob(const RawJob& raw_job);
  // Now 'has_assigned_job_' is set back to false
  // when Input::Update() detects the machine has changed its state from kIdle.
  // It causes potentially a race condition - if the machine processes
  // an assigned job very quick Input::Update() won't notice it.
  bool IsWaitingForAJob() const;

 private:
  RawMachine raw_machine_;
  int context_[3];
  std::shared_ptr<ContextChanges> context_changes_;
  bool has_assigned_job_;
};

}  // namespace greedy
}  // namespace lss

#endif  // LSS_GREEDY_MACHINE_WRAPPER_H_

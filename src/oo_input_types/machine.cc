#include "oo_input_types/machine.h"

namespace lss {

Machine::Machine(io::Machine raw_machine, std::shared_ptr<ContextChanges> context_changes) :
    raw_machine_(std::move(raw_machine)), context_{-1, -1, -1},
    context_changes_(context_changes), has_assigned_job_(false) { }

bool Machine::operator==(const Machine& rhs) const {
  bool eq = true;
  eq &= raw_machine_.id == rhs.raw_machine_.id;
  eq &= has_assigned_job_ == rhs.has_assigned_job_;
  for (int i = 0; i < io::kContextN; ++i)
    eq &= context_[i] == rhs.context_[i];
  return eq;
}

void Machine::SetState(io::MachineState new_state) {
  // FIXME: Now machine must noticed that its state was changed from kIdle
  // otherwise it doesn't work properly
  if (new_state != io::MachineState::kIdle)
    has_assigned_job_ = false;
  raw_machine_.state = new_state;
}

double Machine::ContextChangeCost(const io::Job& raw_job) const {
  bool diff[io::kContextN];
  for (int i = 0; i < io::kContextN; ++i)
    diff[i] = context_[i] != raw_job.context[i];
  return context_changes_->GetContextChangeCost(diff[0], diff[1], diff[2]);
}

int Machine::GetId() const {
  return raw_machine_.id;
}

void Machine::AssignJob(const io::Job& raw_job) {
  has_assigned_job_ = true;
  for (int i = 0; i < io::kContextN; ++i)
    context_[i] = raw_job.context[i];
}

bool Machine::IsWaitingForAJob() const {
  return !has_assigned_job_ && raw_machine_.state == io::MachineState::kIdle;
}

void ContextChanges::SetContextChanges(const std::vector<io::ContextChange> raw_changes) {
  for (const auto& raw_change : raw_changes) {
    const bool* changed = raw_change.changed;
    context_changes_[changed[0]][changed[1]][changed[2]] = raw_change.cost;
  }
}

int ContextChanges::GetContextChangeCost(bool changed0, bool changed1, bool changed2) const {
  return context_changes_[changed0][changed1][changed2];
}


}  // namespace lss

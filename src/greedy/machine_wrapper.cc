#include "greedy/machine_wrapper.h"

namespace lss {
namespace greedy {

MachineWrapper::MachineWrapper(io::RawMachine raw_machine, std::shared_ptr<ContextChanges> context_changes) :
    raw_machine_(std::move(raw_machine)), context_{-1, -1, -1},
    context_changes_(context_changes), has_assigned_job_(false) { }

bool MachineWrapper::operator==(const MachineWrapper& rhs) const {
  return raw_machine_.id == rhs.raw_machine_.id;
}

void MachineWrapper::SetState(io::MachineState new_state) {
  if (new_state != io::MachineState::kIdle)
    has_assigned_job_ = false;
  raw_machine_.state = new_state;
}

double MachineWrapper::ContextChangeCost(const io::RawJob& raw_job) const {
  bool diff[io::kContextN];
  for (int i = 0; i < io::kContextN; ++i)
    diff[i] = context_[i] != raw_job.context[i];
  return context_changes_->GetContextChangeCost(diff[0], diff[1], diff[2]);
}

int MachineWrapper::GetId() const {
  return raw_machine_.id;
}

void MachineWrapper::AssignJob(const io::RawJob& raw_job) {
  has_assigned_job_ = true;
  for (int i = 0; i < io::kContextN; ++i)
    context_[i] = raw_job.context[i];
}

bool MachineWrapper::IsWaitingForAJob() const {
  return !has_assigned_job_ && raw_machine_.state == io::MachineState::kIdle;
}

void ContextChanges::SetContextChanges(const std::vector<io::RawContextChange> raw_changes) {
  for (const auto& raw_change : raw_changes) {
    const bool* changed = raw_change.changed;
    context_changes_[changed[0]][changed[1]][changed[2]] = raw_change.cost;
  }
}

int ContextChanges::GetContextChangeCost(bool changed0, bool changed1, bool changed2) const {
  return context_changes_[changed0][changed1][changed2];
}

}  // namespace greedy
}  // namespace lss

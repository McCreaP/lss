#include "greedy/machine_wrapper.h"

namespace lss {
namespace greedy {

MachineWrapper::MachineWrapper(RawMachine raw_machine,
                               std::shared_ptr<ContextChanges> context_changes) :
    raw_machine_(std::move(raw_machine)), context_{-1, -1, -1},
    context_changes_(context_changes), has_assigned_job_(false) { }

bool MachineWrapper::operator==(const MachineWrapper& rhs) const {
  return raw_machine_.id_ == rhs.raw_machine_.id_;
}

void MachineWrapper::SetState(MachineState new_state) {
  if (new_state != MachineState::kIdle)
    has_assigned_job_ = false;
  raw_machine_.state_ = new_state;
}

double MachineWrapper::ContextChangeCost(const RawJob& raw_job) const {
  bool diff[Context::kSize];
  for (int i = 0; i < Context::kSize; ++i)
    diff[i] = context_[i] != raw_job.context_[i];
  return context_changes_->GetContextChangeCost(diff[0], diff[1], diff[2]);
}

int MachineWrapper::GetId() const {
  return raw_machine_.id_;
}

void MachineWrapper::AssignJob(const RawJob& raw_job) {
  has_assigned_job_ = true;
  for (int i = 0; i < Context::kSize; ++i)
    context_[i] = raw_job.context_[i];
}

bool MachineWrapper::IsWaitingForAJob() const {
  return !has_assigned_job_ && raw_machine_.state_ == MachineState::kIdle;
}

void ContextChanges::SetContextChanges(const std::vector<RawChangeCost> raw_changes) {
  for (const auto& raw_change : raw_changes) {
    const Change& changed = raw_change.change_;
    context_changes_[changed[0]][changed[1]][changed[2]] = raw_change.cost_;
  }
}

int ContextChanges::GetContextChangeCost(bool changed0, bool changed1, bool changed2) const {
  return context_changes_[changed0][changed1][changed2];
}

}  // namespace greedy
}  // namespace lss
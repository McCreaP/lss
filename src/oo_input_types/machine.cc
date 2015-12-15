#include "oo_input_types/machine.h"

namespace lss {

int Machine::context_changes_[2][2][2] = {0, 0, 0, 0, 0, 0, 0, 0};

void Machine::SetContextChanges(const std::vector<io::ContextChange>& raw_changes) {
  for (const auto& raw_change : raw_changes) {
    const bool* changed = raw_change.changed;
    context_changes_[changed[0]][changed[1]][changed[2]] = raw_change.cost;
  }
}

Machine::Machine(io::Machine raw_machine) :
    raw_machine_(std::move(raw_machine)), context_{-1, -1, -1}, job_assigned_(false) { }


bool Machine::operator==(const Machine& rhs) const {
  bool eq = true;
  eq &= raw_machine_.id == rhs.raw_machine_.id;
  eq &= job_assigned_ == rhs.job_assigned_;
  for (int i = 0; i < io::kContextN; ++i)
    eq &= context_[i] == rhs.context_[i];
  return eq;
}

void Machine::SetState(io::MachineState new_state) {
  // FIXME: Now machine must noticed that its state was changed from kIdle
  // otherwise it doesn't work properly
  if (new_state != io::MachineState::kIdle)
    job_assigned_ = false;
  raw_machine_.state = new_state;
}

double Machine::ContextChangeCost(const io::Job& raw_job) const {
  bool diff[io::kContextN];
  for (int i = 0; i < io::kContextN; ++i)
    diff[i] = context_[i] != raw_job.context[i];
  return context_changes_[diff[0]][diff[1]][diff[2]];
}

int Machine::GetId() const {
  return raw_machine_.id;
}

void Machine::AssignJob(const io::Job& raw_job) {
  job_assigned_ = true;
  for (int i = 0; i < io::kContextN; ++i)
    context_[i] = raw_job.context[i];
}

bool Machine::IsWaitingForAJob() const {
  return !job_assigned_ && raw_machine_.state == io::MachineState::kIdle;
}

}  // namespace lss

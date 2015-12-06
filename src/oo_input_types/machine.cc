#include "oo_input_types/machine.h"

namespace lss {

int Machine::context_changes_[2][2][2] = {0, 0, 0, 0, 0, 0, 0, 0};

void Machine::SetContextChanges(const std::vector<io::ContextChange>& raw_changes) {
  for (const auto& raw_change : raw_changes)
    context_changes_[raw_change.changed[0]][raw_change.changed[1]][raw_change.changed[2]] = raw_change.cost;
}

Machine::Machine(const io::Machine& raw_machine) :
    raw_machine_(raw_machine), context_{-1, -1, -1}, job_assigned_(false) { }

void Machine::UpdateState(io::MachineState new_state) {
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

bool Machine::IsIdle() const {
  return raw_machine_.state == io::MachineState::kIdle;
}

}  // namespace lss

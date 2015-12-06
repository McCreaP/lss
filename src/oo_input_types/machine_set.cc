#include "oo_input_types/machine_set.h"

#include "oo_input_types/machine.h"

namespace lss {

void MachineSet::AddMachine(const Machine& machine) {
  if (machine.IsIdle())
    idle_machines_.push_back(machine);
}

std::vector<Machine> MachineSet::GetIdleMachines() const {
  return idle_machines_;
}

}  // namespace lss

#include "oo_input_types/machine_set.h"

#include "oo_input_types/machine.h"

namespace lss {

void MachineSet::AddMachine(const Machine& machine) {
  machines_.push_back(machine);
}

std::vector<Machine> MachineSet::GetMachines() const {
  return machines_;
}

}  // namespace lss

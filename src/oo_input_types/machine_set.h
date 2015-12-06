#ifndef LSS_OO_INPUT_TYPES_MACHINE_SET_H_
#define LSS_OO_INPUT_TYPES_MACHINE_SET_H_

#include <vector>

#include "oo_input_types/machine.h"

namespace lss {

class MachineSet {
 public:
  void AddMachine(const Machine& machine);
  std::vector<Machine> GetIdleMachines() const;

 private:
  std::vector<Machine> idle_machines_;
};

}  // namespace lss

#endif  // LSS_OO_INPUT_TYPES_MACHINE_SET_H_

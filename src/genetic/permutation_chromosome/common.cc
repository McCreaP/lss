#include "genetic/permutation_chromosome/common.h"

#include "base/situation.h"

namespace lss {
namespace genetic {

const Machine *findRandomMachineForJob(const Situation &situation, const Job &job) {
  const std::vector<Machine> &availableMachines = job.machine_set().machines();
  size_t index = std::rand() % availableMachines.size();
  return &availableMachines[index];
}

}  // namespace genetic
}  // namespace lss

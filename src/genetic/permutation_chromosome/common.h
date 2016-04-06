#ifndef LSS_GENETIC_PERMUTATION_CHROMOSOME_COMMON_H_
#define LSS_GENETIC_PERMUTATION_CHROMOSOME_COMMON_H_

#include <vector>

#include "base/situation.h"

namespace lss {
namespace genetic {

const Machine *FindRandomMachineForJob(const Job &job, Random *rand) {
  const std::vector<Machine> &availableMachines = job.machine_set().machines();
  size_t index = rand->Rand(availableMachines.size());
  return &availableMachines[index];
}

}  // namespace genetic
}  // namespace lss

#endif  // LSS_GENETIC_PERMUTATION_CHROMOSOME_COMMON_H_

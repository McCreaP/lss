#include "genetic/permutation_chromosome/common.h"

#include "base/random.h"

namespace lss {
namespace genetic {

Machine FindRandomMachineForJob(Job job, Random *rand) {
  const std::vector<Machine> &available_machines = job.machine_set().machines();
  size_t index = rand->Rand(available_machines.size());
  return available_machines[index];
}

}  // namespace genetic
}  // namespace lss

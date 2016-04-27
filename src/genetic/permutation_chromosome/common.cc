#include "genetic/permutation_chromosome/common.h"

#include "base/random.h"
#include "genetic/permutation_chromosome/chromosome.h"

namespace lss {
namespace genetic {

Machine FindRandomMachineForJob(Job job, Random *rand) {
  MachineSet::Machines available_machines = job.machine_set().machines();
  size_t index = rand->Rand(available_machines.size());
  return available_machines[index];
}

std::vector<JobMachine> GetPermutation(const std::vector<int> &job_permutation,
                                       const std::vector<int> &machines,
                                       Situation situation) {
  std::vector<JobMachine> permutation;
  for (size_t i = 0; i < job_permutation.size(); ++i) {
    Job j = situation.jobs()[job_permutation[i]];
    Machine m = situation.machines()[machines[i]];
    JobMachine job_machine = std::make_tuple(j, m);
    permutation.push_back(job_machine);
  }
  return permutation;
}

}  // namespace genetic
}  // namespace lss

#include <tuple>

#include "genetic/permutation_chromosome/chromosome.h"
#include "genetic/permutation_chromosome/common.h"
#include "genetic/permutation_chromosome/moves_impl.h"

namespace lss {
namespace genetic {

void MutatorImpl::Mutate(__attribute__((unused)) Situation situation,
                         PermutationJobMachine *chromosome) const {
  for (size_t i = 0; i < chromosome->permutation().size(); ++i) {
    bool take_job_machine_to_mutation = rand_->GetRealInRange(0., 1.) < kMutationProbability;
    if (take_job_machine_to_mutation) {
      Job job = std::get<0>(chromosome->permutation()[i]);
      Machine machine = FindRandomMachineForJob(job, rand_.get());
      chromosome->permutation()[i] = std::make_tuple(job, machine);
    }
  }
}

}  // namespace genetic
}  // namespace lss

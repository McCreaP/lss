#include <tuple>

#include "genetic/permutation_chromosome/moves_impl.h"
#include "genetic/permutation_chromosome/chromosome.h"
#include "genetic/permutation_chromosome/common.h"

namespace lss {
namespace genetic {

void MutatorImpl::Mutate(__attribute__((unused)) const Situation &situation, PermutationJobMachine *chromosome) const {
  for (size_t i = 0; i < chromosome->permutation().size(); ++i) {
    bool takeJobMachineToMutation = rand_->GetRealInRange(0., 1.) < kMutationProbability;
    if (takeJobMachineToMutation) {
      const Job *job = std::get<0>(chromosome->permutation()[i]);
      const Machine *machine = FindRandomMachineForJob(*job, rand_.get());
      chromosome->permutation()[i] = std::make_tuple(job, machine);
    }
  }
}

}  // namespace genetic
}  // namespace lss

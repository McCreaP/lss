#include <tuple>

#include "base/utils.h"
#include "genetic/permutation_chromosome/moves_impl.h"
#include "genetic/permutation_chromosome/chromosome.h"
#include "genetic/permutation_chromosome/common.h"

namespace lss {
namespace genetic {

void MutatorImpl::mutate(const Situation &situation, Chromosome *chromosome) const {
  for (size_t i = 0; i < chromosome->size(); ++i) {
    bool takeJobMachineToMutation = getRandInRange(0., 1.) < kMutationProbability;
    if (takeJobMachineToMutation) {
      const Job *job = std::get<0>((*chromosome)[i]);
      const Machine *machine = findRandomMachineForJob(situation, &job);
      (*chromosome)[i] = std::make_tuple(job, machine);
    }
  }
}

}  // namespace genetic
}  // namespace lss
#include <algorithm>
#include <tuple>

#include "base/situation.h"
#include "genetic/moves.h"
#include "genetic/permutation_chromosome/moves_impl.h"
#include "genetic/permutation_chromosome/chromosome.h"
#include "genetic/permutation_chromosome/common.h"

namespace lss {
namespace genetic {

Population<PermutationJobMachine> InitializerImpl::InitPopulation(const Situation &situation, int populationSize) const {
  Population<PermutationJobMachine> population;
  for (int i = 0; i < populationSize; ++i) {
    PermutationJobMachine chromosome = GenNewChromosome(situation);
    population.push_back(std::move(chromosome));
  }
  return population;
}

PermutationJobMachine InitializerImpl::GenNewChromosome(const Situation &situation) const {
  std::vector<const Job *> jobsPermutation;
  for (const Job &job : situation.jobs()) {
    jobsPermutation.push_back(&job);
  }
  std::random_shuffle(std::begin(jobsPermutation), std::end(jobsPermutation));

  PermutationJobMachine chromosome;
  for (const Job *job : jobsPermutation) {
    const Machine *machine = FindRandomMachineForJob(*job, rand_.get());
    chromosome.permutation().push_back(std::make_tuple(job, machine));
  }
  return chromosome;
}

}  // namespace genetic
}  // namespace lss

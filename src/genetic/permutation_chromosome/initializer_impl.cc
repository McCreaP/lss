#include <algorithm>
#include <tuple>

#include "base/situation.h"
#include "genetic/moves.h"
#include "genetic/permutation_chromosome/chromosome.h"
#include "genetic/permutation_chromosome/common.h"
#include "genetic/permutation_chromosome/moves_impl.h"

namespace lss {
namespace genetic {

Population<PermutationJobMachine> InitializerImpl::InitPopulation(const Situation &situation,
                                                                  int population_size) const {
  Population<PermutationJobMachine> population;
  for (int i = 0; i < population_size; ++i) {
    PermutationJobMachine chromosome = GenNewChromosome(situation);
    population.push_back(std::move(chromosome));
  }
  return population;
}

PermutationJobMachine InitializerImpl::GenNewChromosome(const Situation &situation) const {
  std::vector<Job> jobs_permutation;
  for (Job job : situation.jobs()) {
    jobs_permutation.push_back(job);
  }
  std::random_shuffle(std::begin(jobs_permutation), std::end(jobs_permutation));

  PermutationJobMachine chromosome;
  for (Job job : jobs_permutation) {
    Machine machine = FindRandomMachineForJob(job, rand_.get());
    chromosome.permutation().push_back(std::make_tuple(job, machine));
  }
  return chromosome;
}

}  // namespace genetic
}  // namespace lss

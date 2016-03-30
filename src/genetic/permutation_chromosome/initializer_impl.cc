#include <algorithm>
#include <tuple>

#include "base/situation.h"
#include "genetic/moves.h"
#include "genetic/permutation_chromosome/chromosome.h"
#include "genetic/permutation_chromosome/common.h"

namespace lss {
namespace genetic {

std::vector<JobMachine> genNewChromosome(const Situation &situation);
const Machine *findRandomMachineForJob(const lss::Situation &situation, const Job &job);

Population<Chromosome> Initializer::initPopulation(const Situation &situation, int populationSize) const {
  Population<Chromosome> population;
  for (int i = 0; i < populationSize; ++i) {
    Chromosome chromosome = genNewChromosome(situation);
    population.push_back(std::move(chromosome));
  }
  return population;
}

namespace {

std::vector<JobMachine> genNewChromosome(const Situation &situation) {
  std::vector<const Job *> jobsPermutation;
  for (const Job &job : situation.jobs()) {
    jobsPermutation.push_back(&job);
  }
  std::random_shuffle(std::begin(jobsPermutation), std::end(jobsPermutation));

  Chromosome chromosome;
  for (const Job *job : jobsPermutation) {
    const Machine *machine = findRandomMachineForJob(situation, *job);
    chromosome.push_back(std::make_tuple(job, machine));
  }
  return chromosome;
}

}  // namespace
}  // namespace genetic
}  // namespace lss



#include "genetic/algorithm.h"

#include <algorithm>

#include "base/situation.h"
#include "base/utils.h"
#include "genetic/moves.h"

namespace lss {
namespace genetic {

template<class T>
Schedule GeneticAlgorithm<T>::run(const Schedule &prevSchedule, const Situation &situation) {
  static_assert(std::is_base_of<Chromosome, T>::value,
                "Genetic algorithm template should be chromosome specialized");
  ChromosomeImprover<T> improver;
  Population<T> population = kMoves.kInitializer.initPopulation(situation, kPopulationSize);
  for (int generation = 0; generation < kNumberOfGenerations; ++generation) {
    population = kMoves.kSelector.select(population, &improver);
    crossover(&population);
    mutate(situation, &population);
  }
  return improver.getBestChromosome().toSchedule();
}

template<class T>
void GeneticAlgorithm<T>::crossover(Population<T> *population) {
  std::random_shuffle(std::begin(*population), std::end(*population));
  T *chromosomeWaitingForCrossover = nullptr;
  for (T *chromosome : *population) {
    bool takeToCrossover = getRandInRange(0., 1.) < kCrossoverProbability;
    if (takeToCrossover) {
      if (chromosomeWaitingForCrossover) {
        kMoves.kCrosser.crossover(chromosomeWaitingForCrossover, chromosome);
        chromosomeWaitingForCrossover = nullptr;
      } else {
        chromosomeWaitingForCrossover = chromosome;
      }
    }
  }
}

template<class T>
void GeneticAlgorithm<T>::mutate(const Situation &situation, Population<T> *population) {
  for (T *chromosome : *population) {
    kMoves.kMutator.mutate(situation, chromosome);
  }
}

}  // namespace genetic
}  // namespace lss

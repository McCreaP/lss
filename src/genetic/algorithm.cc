#include "genetic/algorithm.h"

#include <algorithm>

#include "base/situation.h"
#include "base/utils.h"
#include "genetic/moves.h"

namespace lss {
namespace genetic {

template<class T>
Schedule GeneticAlgorithm<T>::Run(const Schedule &prevSchedule, const Situation &situation) {
  static_assert(std::is_base_of<Chromosome, T>::value,
                "Genetic algorithm template should be chromosome specialized");
  ChromosomeImprover<T> improver;
  Population<T> population = kMoves.kInitializer.InitPopulation(situation, kPopulationSize);
  for (int generation = 0; generation < kNumberOfGenerations; ++generation) {
    population = kMoves.kSelector.Select(population, &improver);
    Crossover(&population);
    Mutate(situation, &population);
  }
  return improver.GetBestChromosome().toSchedule();
}

template<class T>
void GeneticAlgorithm<T>::Crossover(Population<T> *population) {
  std::random_shuffle(std::begin(*population), std::end(*population));
  T *chromosomeWaitingForCrossover = nullptr;
  for (T *chromosome : *population) {
    bool takeToCrossover = GetRandInRange(0., 1.) < kCrossoverProbability;
    if (takeToCrossover) {
      if (chromosomeWaitingForCrossover) {
        kMoves.kCrosser.Crossover(chromosomeWaitingForCrossover, chromosome);
        chromosomeWaitingForCrossover = nullptr;
      } else {
        chromosomeWaitingForCrossover = chromosome;
      }
    }
  }
}

template<class T>
void GeneticAlgorithm<T>::Mutate(const Situation &situation, Population<T> *population) {
  for (T *chromosome : *population) {
    kMoves.kMutator.Mutate(situation, chromosome);
  }
}

}  // namespace genetic
}  // namespace lss

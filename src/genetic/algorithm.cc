#include "algorithm.h"

#include <algorithm>

#include "base/situation.h"
#include "base/utils.h"
#include "genetic/moves.h"

namespace lss {
namespace genetic {

template<class T>
GeneticAlgorithm<T>::run(const Schedule &prevSchedule, const Situation &situation) {
  Population<T> population = kInitializer.initPopulation(situation, kPopulationSize);
  for (int generation = 0; generation < kNumberOfGenerations; ++generation) {
    population = kSelector.select(population);
    crossover(&population);
    mutate(&population);
  }
}

template<class T>
void GeneticAlgorithm<T>::crossover(Population<T> *population) {
  std::random_shuffle(std::begin(*population), std::end(*population));
  T *chromosomeWaitingForCrossover = nullptr;
  for (T *chromosome : *population) {
    bool takeToCrossover = getRandInRange(0., 1.) < kCrossoverProbability;
    if (takeToCrossover) {
      if (chromosomeWaitingForCrossover) {
        kCrosser.crossover(chromosomeWaitingForCrossover, chromosome);
        chromosomeWaitingForCrossover = nullptr;
      } else {
        chromosomeWaitingForCrossover = chromosome;
      }
    }
  }
}

template<class T>
void GeneticAlgorithm<T>::mutate(Population<T> *population) {
  for (T *chromosome : *population) {
    kMutator.mutate(chromosome);
  }
}

}  // namespace genetic
}  // namespace lss

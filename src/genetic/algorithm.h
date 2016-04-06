#ifndef LSS_GENETIC_ALGORITHM_H_
#define LSS_GENETIC_ALGORITHM_H_

#include <utility>
#include <vector>
#include <algorithm>
#include <iostream>

#include "base/algorithm.h"
#include "base/situation.h"
#include "base/schedule_mock.h"
#include "base/random.h"
#include "genetic/moves.h"

namespace lss {
namespace genetic {

class Chromosome {
 public:
  Chromosome() = default;
  virtual Chromosome &operator=(const Chromosome &other) = default;
  virtual Schedule ToSchedule() const = 0;
  virtual ~Chromosome() = default;
};

template<class T>
class GeneticAlgorithm: public Algorithm {
 public:
  GeneticAlgorithm(int populationSize,
                   int numberOfGenerations,
                   double crossoverProbability,
                   std::shared_ptr<Moves<T>> moves,
                   std::shared_ptr<Random> rand)
      : populationSize_(populationSize),
        numberOfGenerations_(numberOfGenerations),
        crossoverProbability_(crossoverProbability),
        moves_(moves),
        rand_(rand) {}

  Schedule Run(const Schedule &prevSchedule, const Situation &situation) override;

 private:
  void Crossover(Population<T> *population);
  void Mutate(const Situation &situation, Population<T> *population);

  int populationSize_;
  int numberOfGenerations_;
  double crossoverProbability_;
  std::shared_ptr<Moves<T>> moves_;
  std::shared_ptr<Random> rand_;
};

template<class T>
Schedule GeneticAlgorithm<T>::Run(__attribute__((unused)) const Schedule &prevSchedule, const Situation &situation) {
  ChromosomeImprover<T> improver;
  Population<T> population = moves_->InitPopulation(situation, populationSize_);
  for (int generation = 0; generation < numberOfGenerations_; ++generation) {
    population = moves_->Select(population, &improver);
    Crossover(&population);
    Mutate(situation, &population);
  }
  return improver.GetBestChromosome().ToSchedule();
}

template<class T>
void GeneticAlgorithm<T>::Crossover(Population<T> *population) {
  std::vector<size_t> indexes(population->size());
  std::iota(std::begin(indexes), std::end(indexes), 0);
  rand_->RandomShuffle(&indexes);
  T *chromosomeWaitingForCrossover = nullptr;
  for (size_t i = 0; i < population->size(); ++i) {
    bool takeToCrossover = rand_->GetRealInRange(0., 1.) < crossoverProbability_;
    if (takeToCrossover) {
      if (chromosomeWaitingForCrossover) {
        moves_->Crossover(chromosomeWaitingForCrossover, &(*population)[indexes[i]]);
        chromosomeWaitingForCrossover = nullptr;
      } else {
        chromosomeWaitingForCrossover = &(*population)[indexes[i]];
      }
    }
  }
}

template<class T>
void GeneticAlgorithm<T>::Mutate(const Situation &situation, Population<T> *population) {
  for (T &chromosome : *population) {
    moves_->Mutate(situation, &chromosome);
  }
}

}  // namespace genetic
}  // namespace lss

#endif  // LSS_GENETIC_ALGORITHM_H_

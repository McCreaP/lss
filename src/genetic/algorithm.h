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
                   Moves<T> moves,
                   Random rand)
      : populationSize_(populationSize),
        numberOfGenerations_(numberOfGenerations),
        crossoverProbability_(crossoverProbability),
        moves_(std::make_shared<Moves<T>>(std::move(moves))),
        rand_(std::make_shared<Random>(std::move(rand))) {}

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
  static_assert(std::is_base_of<Chromosome, T>::value,
                "Genetic algorithm template should be chromosome specialized");
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
  std::random_shuffle(std::begin(*population), std::end(*population));
  T *chromosomeWaitingForCrossover = nullptr;
  for (T &chromosome : *population) {
    bool takeToCrossover = rand_->GetRealInRange(0., 1.) < crossoverProbability_;
    if (takeToCrossover) {
      if (chromosomeWaitingForCrossover) {
        moves_->Crossover(chromosomeWaitingForCrossover, &chromosome);
        chromosomeWaitingForCrossover = nullptr;
      } else {
        chromosomeWaitingForCrossover = &chromosome;
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

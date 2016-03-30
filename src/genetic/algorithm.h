#ifndef LSS_GENETIC_ALGORITHM_H
#define LSS_GENETIC_ALGORITHM_H

#include <utility>
#include <vector>

#include "base/algorithm.h"
#include "base/situation.h"
#include "genetic/moves.h"

namespace lss {
namespace genetic {

template<class T>
class GeneticAlgorithm: public Algorithm {
 public:
  GeneticAlgorithm(int populationSize,
                   int numberOfGenerations,
                   double crossoverProbability,
                   Initializer<T> initializer,
                   Selector<T> selector,
                   Crosser<T> crosser,
                   Mutator<T> mutator)
      : kPopulationSize(populationSize),
        kNumberOfGenerations(numberOfGenerations),
        kCrossoverProbability(crossoverProbability),
        kInitializer(initializer),
        kSelector(std::move(selector)),
        kCrosser(std::move(crosser)),
        kMutator(std::move(mutator)) { }

  Schedule run(const Schedule &prevSchedule, const Situation &situation) override;

 private:
  void crossover(Population<T> *population);
  void mutate(Population<T> *population);

  const int kPopulationSize;
  const int kNumberOfGenerations;
  const double kCrossoverProbability;
  const Initializer<T> kInitializer;
  const Selector<T> kSelector;
  const Crosser<T> kCrosser;
  const Mutator<T> kMutator;
};

}  // namespace genetic
}  // namespace lss

#endif  // LSS_GENETIC_ALGORITHM_H

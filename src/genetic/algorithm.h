#ifndef LSS_GENETIC_ALGORITHM_H_
#define LSS_GENETIC_ALGORITHM_H_

#include <utility>
#include <vector>

#include "base/algorithm.h"
#include "base/situation.h"
#include "base/schedule_mock.h"
#include "genetic/moves.h"

namespace lss {
namespace genetic {

class Chromosome {
 public:
  Chromosome() = default;
  virtual Schedule toSchedule() const = 0;
};

template<class T>
class GeneticAlgorithm: public Algorithm {
 public:
  GeneticAlgorithm(int populationSize,
                   int numberOfGenerations,
                   double crossoverProbability,
                   Moves<T> moves)
      : kPopulationSize(populationSize),
        kNumberOfGenerations(numberOfGenerations),
        kCrossoverProbability(crossoverProbability),
        kMoves(std::move(moves)) {}

  Schedule run(const Schedule &prevSchedule, const Situation &situation) override;

 private:
  void crossover(Population<T> *population);
  void mutate(const Situation &situation, Population<T> *population);

  const int kPopulationSize;
  const int kNumberOfGenerations;
  const double kCrossoverProbability;
  const Moves<T> kMoves;
};

}  // namespace genetic
}  // namespace lss

#endif  // LSS_GENETIC_ALGORITHM_H_

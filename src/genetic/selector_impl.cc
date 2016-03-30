#include "selector_impl.h"

#include <vector>

#include "base/utils.h"
#include "moves.h"

namespace lss {
namespace genetic {

template<class T>
std::vector<double> calculateCumulativeFitness(const Population<T> &population);
int getAPosition(const std::vector<double> cumulativeFitness);

template<class T>
Population<T> SelectorImpl<T>::select(const Population<T> &population) const {
  std::vector<double> cumulativeFitness = calculateCumulativeFitness(population);
  Population<T> newPopulation;
  for (int i = 0; i < population.size(); ++i) {
    int position = getAPosition(cumulativeFitness);
    newPopulation.push_back(population[position]);
  }
  return newPopulation;
}

namespace {

template<class T>
std::vector<double> SelectorImpl<T>::calculateCumulativeFitness(const Population<T> &population) const {
  std::vector<double> fitnesses;
  for (const T &chromosome : population) {
    fitnesses.push_back(kEvaluator.evaluate(chromosome));
  }

  std::vector<double> cumulativeFitness;
  double accumulator = 0.;
  for (double fitness : fitnesses) {
    accumulator += fitness;
    cumulativeFitness.push_back(accumulator);
  }
  return cumulativeFitness;
}

int getAPosition(const std::vector<double> cumulativeFitness) {
  double rand = getRandInRange(0, cumulativeFitness.back());
  auto low = std::lower_bound(std::begin(cumulativeFitness), std::end(cumulativeFitness), rand);
  return low - std::begin(cumulativeFitness);
}

}  // namespace
}  // namespace genetic
}  // namespace lss

#include "genetic/selector_impl.h"

#include <vector>
#include <tuple>

#include "base/utils.h"
#include "genetic/moves.h"

namespace lss {
namespace genetic {
namespace {

int selectChromosomeIndex(const std::vector<double> &cumulativeFitness) {
  double rand = getRandInRange(0, cumulativeFitness.back());
  auto low = std::lower_bound(std::begin(cumulativeFitness), std::end(cumulativeFitness), rand);
  return low - std::begin(cumulativeFitness);
}

}  // namespace

template<class T>
Population<T> SelectorImpl<T>::select(const Population<T> &population,
                                      ChromosomeImprover<T> *improver) const {
  std::vector<double> cumulativeFitness = calcCumulativeFitness(population, improver);
  Population<T> newPopulation;
  for (int i = 0; i < population.size(); ++i) {
    int index = selectChromosomeIndex(cumulativeFitness);
    newPopulation.push_back(population[index]);
  }
  return std::make_tuple(newPopulation, bestChromosome_);
}

template<class T>
std::vector<double> SelectorImpl<T>::calcCumulativeFitness(const Population<T> &population,
                                                           ChromosomeImprover<T> *improver) const {
  std::vector<double> fitnesses;
  ChromosomeImprover<T> populationImprover;
  for (const T &chromosome : population) {
    double fitness = kEvaluator.evaluate(chromosome);
    populationImprover.tryImprove(chromosome, fitness);
    fitnesses.push_back(fitness);
  }
  improver->tryImprove(populationImprover);

  std::vector<double> cumulativeFitness;
  double accumulator = 0.;
  for (double fitness : fitnesses) {
    accumulator += fitness;
    cumulativeFitness.push_back(accumulator);
  }
  return cumulativeFitness;
}

template<class T>
void ChromosomeImprover<T>::tryImprove(const T &chromosome, double fitness) {
  if (fitness > bestFitness_) {
    bestFitness_ = fitness;
    bestChromosome_ = chromosome;
  }
}

template<class T>
void ChromosomeImprover<T>::tryImprove(const ChromosomeImprover<T> &other) {
  tryImprove(other.getBestChromosome(), other.getBestFitness());
}

}  // namespace genetic
}  // namespace lss

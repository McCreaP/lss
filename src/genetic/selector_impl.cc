#include "genetic/selector_impl.h"

#include <vector>
#include <tuple>

#include "base/utils.h"
#include "genetic/moves.h"

namespace lss {
namespace genetic {
namespace {

int selectChromosomeIndex(const std::vector<double> &cumulativeFitness) {
  double rand = GetRandInRange(0, cumulativeFitness.back());
  auto low = std::lower_bound(std::begin(cumulativeFitness), std::end(cumulativeFitness), rand);
  return low - std::begin(cumulativeFitness);
}

}  // namespace

template<class T>
Population<T> SelectorImpl<T>::Select(const Population<T> &population,
                                      ChromosomeImprover<T> *improver) const {
  std::vector<double> cumulativeFitness = CalcCumulativeFitness(population, improver);
  Population<T> newPopulation;
  for (int i = 0; i < population.size(); ++i) {
    int index = selectChromosomeIndex(cumulativeFitness);
    newPopulation.push_back(population[index]);
  }
  return std::make_tuple(newPopulation, bestChromosome_);
}

template<class T>
std::vector<double> SelectorImpl<T>::CalcCumulativeFitness(const Population<T> &population,
                                                           ChromosomeImprover<T> *improver) const {
  std::vector<double> fitnesses;
  ChromosomeImprover<T> populationImprover;
  for (const T &chromosome : population) {
    double fitness = kEvaluator.Evaluate(chromosome);
    populationImprover.TryImprove(chromosome, fitness);
    fitnesses.push_back(fitness);
  }
  improver->TryImprove(populationImprover);

  std::vector<double> cumulativeFitness;
  double accumulator = 0.;
  for (double fitness : fitnesses) {
    accumulator += fitness;
    cumulativeFitness.push_back(accumulator);
  }
  return cumulativeFitness;
}

template<class T>
void ChromosomeImprover<T>::TryImprove(const T &chromosome, double fitness) {
  if (fitness > bestFitness_) {
    bestFitness_ = fitness;
    bestChromosome_ = chromosome;
  }
}

template<class T>
void ChromosomeImprover<T>::TryImprove(const ChromosomeImprover<T> &other) {
  TryImprove(other.GetBestChromosome(), other.GetBestFitness());
}

}  // namespace genetic
}  // namespace lss

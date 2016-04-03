#ifndef LSS_GENETIC_SELECTOR_IMPL_H_
#define LSS_GENETIC_SELECTOR_IMPL_H_

#include <utility>
#include <vector>

#include "base/schedule_mock.h"
#include "base/random.h"
#include "genetic/moves.h"
#include "genetic/algorithm.h"

namespace lss {
namespace genetic {

template<class T>
class SelectorImpl: public Selector<T> {
 public:
  SelectorImpl(std::shared_ptr<Evaluator<T>> evaluator, std::shared_ptr<Random> rand)
      : kEvaluator(evaluator), rand_(rand) {}

  Population<T> Select(const Population<T> &population,
                       ChromosomeImprover<T> *improver) const override;

 private:
  const std::shared_ptr<Evaluator<T>> kEvaluator;
  T bestChromosome_;
  std::shared_ptr<Random> rand_;

  std::vector<double> CalcCumulativeFitness(const Population<T> &population,
                                            ChromosomeImprover<T> *improver) const;
  size_t selectChromosomeIndex(const std::vector<double> &cumulativeFitness) const;
};

template<class T>
Population<T> SelectorImpl<T>::Select(const Population<T> &population,
                                      ChromosomeImprover<T> *improver) const {
  std::vector<double> cumulativeFitness = CalcCumulativeFitness(population, improver);
  Population<T> newPopulation;
  for (size_t i = 0; i < population.size(); ++i) {
    size_t index = selectChromosomeIndex(cumulativeFitness);
    newPopulation.push_back(population[index]);
  }
  return newPopulation;
}

template<class T>
std::vector<double> SelectorImpl<T>::CalcCumulativeFitness(const Population<T> &population,
                                                           ChromosomeImprover<T> *improver) const {
  std::vector<double> fitnesses;
  ChromosomeImprover<T> populationImprover;
  for (const T &chromosome : population) {
    double fitness = kEvaluator->Evaluate(chromosome);
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
size_t SelectorImpl<T>::selectChromosomeIndex(const std::vector<double> &cumulativeFitness) const {
  double rand = rand_->GetRealInRange(0, cumulativeFitness.back());
  auto low = std::lower_bound(std::begin(cumulativeFitness), std::end(cumulativeFitness), rand);
  return low - std::begin(cumulativeFitness);
}

template<class T>
void ChromosomeImprover<T>::TryImprove(const T &chromosome, double fitness) {
  if (fitness > bestFitness_) {
    bestFitness_ = fitness;
    bestChromosome_ = std::make_shared<T>(chromosome);
  }
}

template<class T>
void ChromosomeImprover<T>::TryImprove(const ChromosomeImprover<T> &other) {
  TryImprove(other.GetBestChromosome(), other.GetBestFitness());
}

}  // namespace genetic
}  // namespace lss

#endif  // LSS_GENETIC_SELECTOR_IMPL_H_

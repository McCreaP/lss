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
class SelectorImpl : public Selector<T> {
 public:
  SelectorImpl(std::shared_ptr<Evaluator<T>> evaluator, std::shared_ptr<Random> rand)
      : kEvaluator(evaluator), rand_(rand) {}

  Population<T> Select(const Population<T> &population,
                       ChromosomeImprover<T> *improver) const override;

 private:
  const std::shared_ptr<Evaluator<T>> kEvaluator;
  T best_chromosome_;
  std::shared_ptr<Random> rand_;

  std::vector<double> CalcCumulativeFitness(const Population<T> &population,
                                            ChromosomeImprover<T> *improver) const;
  size_t SelectChromosomeIndex(const std::vector<double> &cumulative_fitness) const;
};

template<class T>
Population<T> SelectorImpl<T>::Select(const Population<T> &population,
                                      ChromosomeImprover<T> *improver) const {
  std::vector<double> cumulative_fitness = CalcCumulativeFitness(population, improver);
  Population<T> new_population;
  for (size_t i = 0; i < population.size(); ++i) {
    size_t index = SelectChromosomeIndex(cumulative_fitness);
    new_population.push_back(population[index]);
  }
  return new_population;
}

template<class T>
std::vector<double> SelectorImpl<T>::CalcCumulativeFitness(const Population<T> &population,
                                                           ChromosomeImprover<T> *improver) const {
  std::vector<double> fitnesses;
  ChromosomeImprover<T> population_improver;
  for (const T &chromosome : population) {
    double fitness = kEvaluator->Evaluate(chromosome);
    population_improver.TryImprove(chromosome, fitness);
    fitnesses.push_back(fitness);
  }
  improver->TryImprove(population_improver);

  std::vector<double> cumulative_fitness;
  double accumulator = 0.;
  for (double fitness : fitnesses) {
    accumulator += fitness;
    cumulative_fitness.push_back(accumulator);
  }
  return cumulative_fitness;
}

template<class T>
size_t SelectorImpl<T>::SelectChromosomeIndex(const std::vector<double> &cumulative_fitness) const {
  double rand = rand_->GetRealInRange(0, cumulative_fitness.back());
  auto low = std::lower_bound(std::begin(cumulative_fitness), std::end(cumulative_fitness), rand);
  return low - std::begin(cumulative_fitness);
}

template<class T>
void ChromosomeImprover<T>::TryImprove(const T &chromosome, double fitness) {
  if (fitness > best_fitness_) {
    best_fitness_ = fitness;
    best_chromosome_ = std::make_shared<T>(chromosome);
  }
}

template<class T>
void ChromosomeImprover<T>::TryImprove(const ChromosomeImprover<T> &other) {
  TryImprove(other.GetBestChromosome(), other.GetBestFitness());
}

}  // namespace genetic
}  // namespace lss

#endif  // LSS_GENETIC_SELECTOR_IMPL_H_

#ifndef LSS_GENETIC_SELECTOR_IMPL_H_
#define LSS_GENETIC_SELECTOR_IMPL_H_

#include <utility>
#include <vector>

#include "base/schedule_mock.h"
#include "genetic/moves.h"
#include "genetic/algorithm.h"

namespace lss {
namespace genetic {

template<class T>
class SelectorImpl: public Selector<T> {
 public:
  explicit SelectorImpl(Evaluator<T> evaluator) : kEvaluator(evaluator) {}
  Population<T> select(const Population<T> &population,
                       ChromosomeImprover<T> *improver) const override;

 private:
  const Evaluator<T> kEvaluator;
  T bestChromosome_;

  std::vector<double> calcCumulativeFitness(const Population<T> &population,
                                            ChromosomeImprover<T> *improver) const;
};

}  // namespace genetic
}  // namespace lss

#endif  // LSS_GENETIC_SELECTOR_IMPL_H_

#ifndef LSS_GENETIC_SELECTOR_IMPL_H_
#define LSS_GENETIC_SELECTOR_IMPL_H_

#include <utility>
#include <vector>
#include "genetic/moves.h"

namespace lss {
namespace genetic {

template<class T>
class SelectorImpl: public Selector {
 public:
  SelectorImpl(Evaluator<T> evaluator) : kEvaluator(evaluator) {}
  Population<T> select(const Population<T> &population) const override;

 private:
  Evaluator<T> kEvaluator;
  std::vector<double> calculateCumulativeFitness(const Population<T> &population) const;
};

}  // namespace genetic
}  // namespace lss

#endif  // LSS_GENETIC_SELECTOR_IMPL_H_

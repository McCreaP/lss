#ifndef LSS_GENETIC_PERMUTATION_CHROMOSOME_MOVES_IMPL_H
#define LSS_GENETIC_PERMUTATION_CHROMOSOME_MOVES_IMPL_H

#include "genetic/moves.h"

namespace lss {
namespace genetic {

class InitializerImpl : public Initializer<Chormosome> {
 public:
  Population<Chormosome> generate(const Situation &situation, int populationSize) const override;
};

class EvaluatorImpl : public Evaluator<Chromosome> {
 public:
  double evaluate(const Chromosome &chromosome) const override;
};

class MutatorImpl : Mutator<Chormosome> {
 public:
  void mutate(const Situation &situation, Chromosome *chromosome) const override;
};

class CrosserImpl: public Crosser<Chromosome> {
 public:
  void crossover(Chromosome *lhs, Chromosome *rhs) const override;
};

}  // namespace genetic
}  // namespace lss

#endif  // LSS_GENETIC_PERMUTATION_CHROMOSOME_MOVES_IMPL_H

#ifndef LSS_GENETIC_PERMUTATION_CHROMOSOME_MOVES_IMPL_H_
#define LSS_GENETIC_PERMUTATION_CHROMOSOME_MOVES_IMPL_H_

#include "genetic/moves.h"
#include "genetic/permutation_chromosome/chromosome.h"

namespace lss {
namespace genetic {

class InitializerImpl : public Initializer<PermutationJobMachine> {
 public:
  explicit InitializerImpl(std::shared_ptr<Random> rand) : rand_(rand) {}
  Population<PermutationJobMachine> InitPopulation(const Situation &situation, int populationSize) const override;

 private:
  std::shared_ptr<Random> rand_;
  PermutationJobMachine GenNewChromosome(const Situation &situation) const;
};

class EvaluatorImpl : public Evaluator<PermutationJobMachine> {
 public:
  double Evaluate(const PermutationJobMachine &chromosome) const override;
};

class MutatorImpl : Mutator<PermutationJobMachine> {
 public:
  MutatorImpl(double mutationProbability, std::shared_ptr<Random> rand)
      : kMutationProbability(mutationProbability), rand_(rand) {}
  void Mutate(const Situation &situation, PermutationJobMachine *chromosome) const override;

 private:
  double kMutationProbability;
  std::shared_ptr<Random> rand_;
};

class CrosserImpl: public Crosser<PermutationJobMachine> {
 public:
  explicit CrosserImpl(std::shared_ptr<Random> rand) : rand_(rand) {}
  void Crossover(PermutationJobMachine *lhs, PermutationJobMachine *rhs) const override;

 private:
  std::shared_ptr<Random> rand_;
};

}  // namespace genetic
}  // namespace lss

#endif  // LSS_GENETIC_PERMUTATION_CHROMOSOME_MOVES_IMPL_H_

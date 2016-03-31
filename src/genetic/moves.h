#ifndef LSS_GENETIC_MOVES_H_
#define LSS_GENETIC_MOVES_H_

#include <vector>

#include "base/situation.h"

namespace lss {
namespace genetic {

template<class T>
using Population = std::vector<T>;

template<class T>
class Initializer {
 public:
  virtual Population<T> InitPopulation(const Situation &situation, int populationSize) const = 0;
};

template<class T>
class Evaluator {
 public:
  virtual double Evaluate(const T &chromosome) const = 0;

  virtual ~Evaluator();
};

template<class T>
class ChromosomeImprover {
 public:
  ChromosomeImprover() = default;

  const T &GetBestChromosome() const {return bestChromosome_;}
  double GetBestFitness() const {return bestFitness_;}
  void TryImprove(const T &chromosome, double fitness);
  void TryImprove(const ChromosomeImprover<T> &other);

 private:
  T bestChromosome_;
  double bestFitness_;
};

template<class T>
class Selector {
  virtual Population<T> Select(const Population<T> &population,
                               ChromosomeImprover<T> *improver) const = 0;

  virtual ~Selector();
};

template<class T>
class Mutator {
 public:
  explicit Mutator(double mutationProbability) : kMutationProbability(mutationProbability) {}

  virtual void Mutate(const Situation &situation, T *chromosome) const = 0;
  virtual ~Mutator();

 protected:
  const double kMutationProbability;
};

template<class T>
class Crosser {
 public:
  virtual void Crossover(T *lhs, T *rhs) const = 0;
  virtual ~Crosser();
};

template<class T>
class Moves {
 public:
  Moves(Initializer<T> initializer, Selector<T> selector, Mutator<T> mutator, Crosser<T> crosser)
      : kInitializer(initializer), kSelector(selector), kMutator(mutator), kCrosser(crosser) {}

 private:
  const Initializer<T> kInitializer;
  const Selector<T> kSelector;
  const Mutator<T> kMutator;
  const Crosser<T> kCrosser;

  friend class GeneticAlgorithm;
};

}  // namespace genetic
}  // namespace lss

#endif  // LSS_GENETIC_MOVES_H_

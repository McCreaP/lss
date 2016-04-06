#ifndef LSS_GENETIC_MOVES_H_
#define LSS_GENETIC_MOVES_H_

#include <limits>
#include <vector>
#include <memory>

#include "base/situation.h"

namespace lss {
namespace genetic {

template<class T>
using Population = std::vector<T>;

template<class T>
class Initializer {
 public:
  virtual Population<T> InitPopulation(const Situation &situation, int populationSize) const = 0;
  virtual ~Initializer() = default;
};

template<class T>
class Evaluator {
 public:
  virtual double Evaluate(const T &chromosome) const = 0;

  virtual ~Evaluator() = default;
};

template<class T>
class ChromosomeImprover {
 public:
  ChromosomeImprover() = default;

  virtual T GetBestChromosome() const {return *bestChromosome_;}
  virtual double GetBestFitness() const {return bestFitness_;}
  virtual void TryImprove(const T &chromosome, double fitness);
  virtual void TryImprove(const ChromosomeImprover<T> &other);

 private:
  std::shared_ptr<T> bestChromosome_ = std::make_shared<T>();
  double bestFitness_ = std::numeric_limits<double>::min();
};

template<class T>
class Selector {
 public:
  virtual Population<T> Select(const Population<T> &population,
                               ChromosomeImprover<T> *improver) const = 0;

  virtual ~Selector() = default;
};

template<class T>
class Mutator {
 public:
  virtual void Mutate(const Situation &situation, T *chromosome) const = 0;

  virtual ~Mutator() = default;
};

template<class T>
class Crosser {
 public:
  virtual void Crossover(T *lhs, T *rhs) const = 0;

  virtual ~Crosser() = default;
};

template<class T>
class Moves {
 public:
  Moves<T> &SetInitializer(std::shared_ptr<Initializer<T>> initializer) {
    initializer_ = initializer;
    return *this;
  }

  Moves<T> &SetSelector(std::shared_ptr<Selector<T>> selector) {
    selector_ = selector;
    return *this;
  }

  Moves<T> &SetMutator(std::shared_ptr<Mutator<T>> mutator) {
    mutator_ = mutator;
    return *this;
  }

  Moves<T> &SetCrosser(std::shared_ptr<Crosser<T>> crosser) {
    crosser_ = crosser;
    return *this;
  }

  virtual Population<T> InitPopulation(const Situation &situation, int populationSize) const {
    return initializer_->InitPopulation(situation, populationSize);
  }

  virtual Population<T> Select(const Population<T> &population, ChromosomeImprover<T> *improver) const {
    return selector_->Select(population, improver);
  }

  virtual void Mutate(const Situation &situation, T *chromosome) const {
    mutator_->Mutate(situation, chromosome);
  }

  virtual void Crossover(T *lhs, T *rhs) const {
    crosser_->Crossover(lhs, rhs);
  }

  virtual ~Moves() = default;

 private:
  std::shared_ptr<Initializer<T>> initializer_;
  std::shared_ptr<Selector<T>> selector_;
  std::shared_ptr<Mutator<T>> mutator_;
  std::shared_ptr<Crosser<T>> crosser_;
};

}  // namespace genetic
}  // namespace lss

#endif  // LSS_GENETIC_MOVES_H_

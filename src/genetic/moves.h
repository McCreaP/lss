#ifndef LSS_GENETIC_MOVES_H_
#define LSS_GENETIC_MOVES_H_

#include <limits>
#include <memory>
#include <vector>

#include "gmock/gmock.h"

#include "base/situation.h"

namespace lss {
namespace genetic {

template<class T>
using Population = std::vector<T>;

template<class T>
class Initializer {
 public:
  virtual Population<T> InitPopulation(Situation situation, int populationSize) const = 0;
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

  virtual T GetBestChromosome() const { return *best_chromosome_; }
  virtual double GetBestFitness() const { return best_fitness_; }
  virtual void TryImprove(const T &chromosome, double fitness);
  virtual void TryImprove(const ChromosomeImprover<T> &other);

 private:
  std::shared_ptr<T> best_chromosome_ = std::make_shared<T>();
  double best_fitness_ = std::numeric_limits<double>::min();
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
  virtual void Mutate(Situation situation, T *chromosome) const = 0;

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
  virtual Population<T> InitPopulation(Situation situation, int populationSize) const = 0;
  virtual Population<T> Select(const Population<T> &population,
                               ChromosomeImprover<T> *improver) const = 0;
  virtual void Mutate(Situation situation, T *chromosome) const = 0;
  virtual void Crossover(T *lhs, T *rhs) const = 0;

  virtual ~Moves() = default;
};

template<class T>
class ConfigurableMoves : public Moves<T> {
 public:
  ConfigurableMoves<T> &SetInitializer(std::shared_ptr<Initializer<T>> initializer) {
    initializer_ = initializer;
    return *this;
  }

  ConfigurableMoves<T> &SetSelector(std::shared_ptr<Selector<T>> selector) {
    selector_ = selector;
    return *this;
  }

  ConfigurableMoves<T> &SetMutator(std::shared_ptr<Mutator<T>> mutator) {
    mutator_ = mutator;
    return *this;
  }

  ConfigurableMoves<T> &SetCrosser(std::shared_ptr<Crosser<T>> crosser) {
    crosser_ = crosser;
    return *this;
  }

  Population<T> InitPopulation(Situation situation, int populationSize) const {
    return initializer_->InitPopulation(situation, populationSize);
  }

  Population<T> Select(const Population<T> &population,
                               ChromosomeImprover<T> *improver) const {
    return selector_->Select(population, improver);
  }

  void Mutate(Situation situation, T *chromosome) const {
    mutator_->Mutate(situation, chromosome);
  }

  void Crossover(T *lhs, T *rhs) const {
    crosser_->Crossover(lhs, rhs);
  }

 private:
  std::shared_ptr<Initializer<T>> initializer_;
  std::shared_ptr<Selector<T>> selector_;
  std::shared_ptr<Mutator<T>> mutator_;
  std::shared_ptr<Crosser<T>> crosser_;
};

template<class T>
class InitializerMock: public Initializer<T> {
 public:
  MOCK_CONST_METHOD2_T(InitPopulation, Population<T>(Situation, int));
};

template<class T>
class EvaluatorMock: public Evaluator<T> {
 public:
  MOCK_CONST_METHOD1_T(Evaluate, double(const T &));
};

template<class T>
class ImproverMock : public ChromosomeImprover<T> {
 public:
  MOCK_CONST_METHOD0_T(GetBestChromosome, T());
  MOCK_CONST_METHOD0_T(GetBestFitness, double());
  MOCK_METHOD2_T(TryImprove, void(const T &, double));
  MOCK_METHOD1_T(TryImprove, void(const ChromosomeImprover<T> &));
};

template<class T>
class SelectorMock: public Selector<T> {
 public:
  MOCK_CONST_METHOD2_T(Select, Population<T>(const Population<T> &, ChromosomeImprover<T> *));
};

template<class T>
class MutatorMock: public Mutator<T> {
 public:
  MutatorMock() = default;
  MOCK_CONST_METHOD2_T(Mutate, void(Situation, T *));
};

template<class T>
class CrosserMock: public Crosser<T> {
 public:
  MOCK_CONST_METHOD2_T(Crossover, void(T *, T *));
};

template<class T>
class MovesMock: public Moves<T> {
 public:
  MOCK_CONST_METHOD2_T(InitPopulation, Population<T>(Situation, int));
  MOCK_CONST_METHOD2_T(Select, Population<T>(const Population<T> &, ChromosomeImprover<T> *));
  MOCK_CONST_METHOD2_T(Mutate, void(Situation, T *));
  MOCK_CONST_METHOD2_T(Crossover, void(T *, T *));
};
}  // namespace genetic
}  // namespace lss

#endif  // LSS_GENETIC_MOVES_H_

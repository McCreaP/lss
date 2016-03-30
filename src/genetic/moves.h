#ifndef LSS_GENETIC_MOVES_H
#define LSS_GENETIC_MOVES_H

namespace lss {
namespace genetic {

using Population<T> = std::vector<T>

template<class T>
class Initializer {
 public:
  virtual Population<T> initPopulation(const Situation& situation, int populationSize) const = 0;
};

template<class T>
class Selector {
 public:
  virtual Population<T> select(const Population<T> &population) const = 0;
  virtual ~Selector();
};

template<class T>
class Evaluator {
 public:
  virtual double evaluate(const T &chromosome) const = 0;

  virtual ~Evaluator();
};

template<class T>
class Mutator {
 public:
  Mutator(double mutationProbability) : kMutationProbability(mutationProbability) {}

  virtual void mutate(const Situation &situation, T *chromosome) const = 0;
  virtual ~Mutator();

 protected:
  const double kMutationProbability;
};

template<class T>
class Crosser {
 public:
  virtual void crossover(T *lhs, T *rhs) const = 0;
  virtual ~Crosser();
};

}  // namespace genetic
}  // namespace lss

#endif  // LSS_GENETIC_MOVES_H

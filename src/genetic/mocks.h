#ifndef LSS_GENETIC_MOCKS_H_
#define LSS_GENETIC_MOCKS_H_

#include <vector>

#include "gmock/gmock.h"

#include "base/schedule_mock.h"
#include "genetic/algorithm.h"

namespace lss {
namespace genetic {

template<class T>
class Iterator {
 public:
  explicit Iterator(std::vector<T> v) : v_(std::move(v)) {}

  T Next() {
    return v_[counter_++];
  }

 private:
  int counter_ = 0;
  std::vector<T> v_;
};

class ChromosomeMock: public Chromosome {
 public:
  ChromosomeMock() : id_(-1) {}
  explicit ChromosomeMock(int id) : id_(id) {}
  ChromosomeMock(const ChromosomeMock &other) : id_(other.id_) {}
  ChromosomeMock &operator=(const ChromosomeMock &other) {
    id_ = other.id_;
    return *this;
  }

  friend bool operator==(const ChromosomeMock &lhs, const ChromosomeMock &rhs) {
    return lhs.id_ == rhs.id_;
  }

  friend std::ostream &operator<<(std::ostream &os, const ChromosomeMock &chromosome) {
    os << "Chromosome: " << chromosome.id_;
    return os;
  }

  MOCK_CONST_METHOD0(ToSchedule, Schedule());

 private:
  int id_;
};

class ChromosomeFake : public Chromosome {
 public:
  ChromosomeFake() : id_(-1) {}
  explicit ChromosomeFake(int id) : id_(id) {}

  bool operator==(const ChromosomeFake &other) const {return id_ == other.id_;}

  friend std::ostream &operator<<(std::ostream &os, const ChromosomeFake &chromosome) {
    os << "Chromosome: " << chromosome.id_;
    return os;
  }

  Schedule ToSchedule() const override {return Schedule();}

 private:
  int id_;
};

template<class T>
class InitializerMock: public Initializer<T> {
 public:
  MOCK_CONST_METHOD2_T(InitPopulation, Population<T>(const Situation &, int));
};

template<class T>
class EvaluatorMock: public Evaluator<T> {
 public:
  EvaluatorMock() {}
  EvaluatorMock(const EvaluatorMock &) {}
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
  MOCK_CONST_METHOD2_T(Mutate, void(const Situation &, T *));
};

template<class T>
class CrosserMock: public Crosser<T> {
 public:
  MOCK_CONST_METHOD2_T(Crossover, void(T *, T *));
};

template<class T>
class MovesMock: public Moves<T> {
 public:
  MOCK_CONST_METHOD2_T(InitPopulation, Population<T>(const Situation &, int));
  MOCK_CONST_METHOD2_T(Select, Population<T>(const Population<T> &, ChromosomeImprover<T> *));
  MOCK_CONST_METHOD2_T(Mutate, void(const Situation &, T *));
  MOCK_CONST_METHOD2_T(Crossover, void(T *, T *));
};

class RandomMock: public Random {
 public:
  MOCK_CONST_METHOD2(GetRealInRange, double(double, double));
  MOCK_CONST_METHOD1(RandomShuffle, void(std::vector<size_t> *));
  MOCK_CONST_METHOD1(Rand, size_t(size_t));
};

}  // namespace genetic
}  // namespace lss

#endif  // LSS_GENETIC_MOCKS_H_

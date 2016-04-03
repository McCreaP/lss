#ifndef LSS_GENETIC_MOCKS_H_
#define LSS_GENETIC_MOCKS_H_

#include "gmock/gmock.h"

namespace lss {
namespace genetic {

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

class RandomMock: public Random {
 public:
  MOCK_CONST_METHOD2(GetRealInRange, double(double, double));
};

}  // genetic
}  // lss

#endif  // LSS_GENETIC_MOCKS_H_

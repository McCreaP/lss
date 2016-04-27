#ifndef LSS_BASE_RANDOM_H_
#define LSS_BASE_RANDOM_H_

#include <algorithm>
#include <cstdlib>
#include <random>
#include <vector>

#include "gmock/gmock.h"

namespace lss {

class Random {
 public:
  Random() : gen_(std::random_device()()) {}

  virtual double GetRealInRange(double from, double to) {
    std::uniform_real_distribution<> dist(from, to);
    return dist(gen_);
  }

  virtual void RandomShuffle(std::vector<size_t> *v) {
    std::random_shuffle(std::begin(*v), std::end(*v));
  }

  virtual size_t Rand(size_t range) {
    return std::rand() % range;
  }

  virtual ~Random() = default;

 private:
  std::mt19937 gen_;
};

class RandomMock: public Random {
 public:
  MOCK_METHOD2(GetRealInRange, double(double, double));
  MOCK_METHOD1_T(RandomShuffle, void(std::vector<size_t> *));
  MOCK_METHOD1(Rand, size_t(size_t));
};

}  // namespace lss

#endif  // LSS_BASE_RANDOM_H_

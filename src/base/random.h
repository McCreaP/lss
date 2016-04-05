#ifndef LSS_BASE_RAND_H_
#define LSS_BASE_RAND_H_

#include <algorithm>
#include <cstdlib>
#include <random>

class Random {
 public:
  virtual double GetRealInRange(double from, double to) const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(from, to);
    return dist(gen);
  }

  void RandomShuffle(std::vector<size_t> *v) const {
    std::random_shuffle(std::begin(*v), std::end(*v));
  }

  virtual ~Random() = default;
};

#endif  // LSS_BASE_RAND_H_

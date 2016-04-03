#ifndef LSS_BASE_RAND_H_
#define LSS_BASE_RAND_H_

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

  virtual ~Random() = default;
};

#endif  // LSS_BASE_RAND_H_

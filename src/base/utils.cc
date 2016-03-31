#include "base/utils.h"

#include <random>

double getRandInRange(double from, double to) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_real_distribution<> dist(from, to);
  return dist(gen);
}

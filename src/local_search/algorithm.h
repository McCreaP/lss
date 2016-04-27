#ifndef LSS_LOCAL_SEARCH_ALGORITHM_H_
#define LSS_LOCAL_SEARCH_ALGORITHM_H_

#include <random>

#include "base/algorithm.h"
#include "base/schedule.h"

namespace lss {
namespace local_search {

class LocalSearchAlgorithm : public Algorithm {
 public:
  LocalSearchAlgorithm(const LocalSearchAlgorithm &) = delete;
  LocalSearchAlgorithm &operator=(const LocalSearchAlgorithm &) = delete;

  // `iterations` is the total number of move attempts per Run() call.
  explicit LocalSearchAlgorithm(int iterations, int seed);

  Schedule Run(const Schedule &, Situation situation) override;

 private:
  const int iterations_;
  std::default_random_engine random_;
};

}  // namespace local_search
}  // namespace lss

#endif  // LSS_LOCAL_SEARCH_ALGORITHM_H_

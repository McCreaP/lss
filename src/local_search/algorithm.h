#ifndef LSS_LOCAL_SEARCH_ALGORITHM_H_
#define LSS_LOCAL_SEARCH_ALGORITHM_H_

#include "base/algorithm.h"
#include "base/schedule.h"

namespace lss {
namespace local_search {

class LocalSearchAlgorithm : public Algorithm {
 public:
  LocalSearchAlgorithm(const LocalSearchAlgorithm &) = delete;
  LocalSearchAlgorithm &operator=(const LocalSearchAlgorithm &) = delete;

  // `n` is the number of iterations per call to Run.
  explicit LocalSearchAlgorithm(int) {}

  Schedule Run(const Schedule &, Situation) override { return Schedule(); }

 private:
  Situation s;
};

}  // namespace local_search
}  // namespace lss

#endif  // LSS_LOCAL_SEARCH_ALGORITHM_H_

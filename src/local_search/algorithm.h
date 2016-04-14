#ifndef LSS_LOCAL_SEARCH_ALGORITHM_H_
#define LSS_LOCAL_SEARCH_ALGORITHM_H_

#include "local_search/base.h"

namespace lss {
namespace local_search {

class LocalSearchAlgorithm : public BaseAlgorithm {
 public:
  // Parameters meaning:
  // - n: maximum number of iterators per call to Run().
  // - k: maximum number of non-improving iterations till the state is reset.
  LocalSearchAlgorithm(int n, int k);

  void UpdateSituation(Situation s) override;
  BaseSchedule Run() override;

 private:
  Situation s;
  int n_, k_;
};

}  // namespace local_search
}  // namespace lss

#endif  // LSS_LOCAL_SEARCH_ALGORITHM_H_

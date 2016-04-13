#ifndef LSS_TEST_UTILS_H
#define LSS_TEST_UTILS_H

#include "base/raw_situation.h"

namespace lss {
namespace genetic {

RawSituation GetSimpleRawSituation(int numberOfJobs, int numberOfMachines);

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

}  // namespace genetic
}  // namespace lss

#endif //LSS_TEST_UTILS_H

#ifndef LSS_BASE_ALGORITHM_H_
#define LSS_BASE_ALGORITHM_H_

#include "base/schedule_mock.h"
#include "base/situation.h"

namespace lss {

class Algorithm {
 public:
  virtual Schedule Run(const Schedule &prev_schedule, const Situation &new_situation) = 0;

  virtual ~Algorithm() = default;
};

}  // namespace lss

#endif  // LSS_BASE_ALGORITHM_H_

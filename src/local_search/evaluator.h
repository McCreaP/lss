#ifndef LSS_LOCAL_SEARCH_EVALUATOR_H_
#define LSS_LOCAL_SEARCH_EVALUATOR_H_

#include "base/situation.h"

namespace lss {
namespace local_search {

class Evaluator {
 public:
  virtual double GetVal() = 0;
  virtual void OnAssign(Job j, Machine prev, Machine now) = 0;

  static Evaluator* Default();

  virtual ~Evaluator() = default;
};

}  // namespace local_search
}  // namespace lss

#endif  // LSS_LOCAL_SEARCH_EVALUATOR_H_

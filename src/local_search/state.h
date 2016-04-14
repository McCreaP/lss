#ifndef LSS_LOCAL_SEARCH_STATE_H_
#define LSS_LOCAL_SEARCH_STATE_H_

#include "base/situation.h"
#include "local_search/evaluator.h"

namespace lss {
namespace local_search {

// To be replaced with `::lss::Schedule`.
class BaseSchedule {};

class State {
 public:
  State();
  State(const State &);
  State& operator=(const State&);

  State(Situation s);

  Situation GetSituation() const;
  BaseSchedule ToSchedule() const;

  // Schedules `j` as the first job for `m` to execute. Has no effect if `j` is null.
  // Moves `j` to list of unassigned jobs if `m` is null.
  void Assign(Job j, Machine m);
};



}  // local_search
}  // lss

#endif  // LSS_LOCAL_SEARCH_STATE_H_

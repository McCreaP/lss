#ifndef LSS_LOCAL_SEARCH_STATE_H_
#define LSS_LOCAL_SEARCH_STATE_H_

#include <cstddef>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "base/situation.h"
#include "base/schedule.h"

namespace lss {
namespace local_search {

class State {
 public:
  State() = default;
  State(const State &) = default;
  State& operator=(const State&) = default;

  explicit State(Situation) {}

  double Evaluate() const { return double(); }
  Schedule ToSchedule() const { return Schedule(); }

  Machine GetMachine(Job) const { return Machine(); }
  size_t GetPos(Job) const { return size_t(); }

  size_t QueueSize(Machine) const { return size_t(); }
  Job QueueBack(Machine) const { return Job(); }

  void Assign(Machine, Job) {}
  void Assign(Machine, Job, size_t) {}

  template<class URNG>
  void Shuffle(Machine) {}
};

}  // namespace local_search
}  // namespace lss

#endif  // LSS_LOCAL_SEARCH_STATE_H_

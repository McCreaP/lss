#include "local_search/algorithm.h"
#include "local_search/state.h"

namespace lss {
namespace local_search {

LocalSearchAlgorithm::LocalSearchAlgorithm(int iterations, int seed)
    : iterations_(iterations), random_(seed) {}

Schedule LocalSearchAlgorithm::Run(const Schedule &, Situation situation) {
  if (situation.jobs().empty())
    return Schedule(situation);

  using range = std::uniform_int_distribution<size_t>;
  auto rand_job = [&]() {
    return situation.jobs()[range(0, situation.jobs().size() - 1)(random_)];
  };
  auto rand_machine = [&](Job j) {
    auto machines = j.machine_set().machines();
    if (machines.empty()) return Machine();
    return machines[range(0, machines.size() - 1)(random_)];
  };

  State state(situation);
  for (int i = 0; i < iterations_; ++i) {
    double eval = state.Evaluate();
    Job job = state.QueueSize(Machine()) > 0 ? state.QueueBack(Machine()) : rand_job();

    Machine old_machine = state.GetMachine(job);
    size_t old_pos = state.GetPos(job);
    state.Assign(Machine(), job);

    Machine new_machine = rand_machine(job);
    size_t new_pos = range(0, state.QueueSize(new_machine) - 1)(random_);
    state.Assign(new_machine, job, new_pos);

    if (state.Evaluate() < eval)
      state.Assign(old_machine, job, old_pos);
  }

  return state.ToSchedule();
}

}  // namespace local_search
}  // namespace lss

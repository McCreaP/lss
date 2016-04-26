#ifndef LSS_GENETIC_PERMUTATION_CHROMOSOME_CHROMOSOME_H_
#define LSS_GENETIC_PERMUTATION_CHROMOSOME_CHROMOSOME_H_

#include <tuple>
#include <vector>

#include "gmock/gmock.h"

#include "base/situation.h"
#include "genetic/algorithm.h"

namespace lss {
namespace genetic {

using JobMachine = std::tuple<Job, Machine>;

class PermutationJobMachine : public Chromosome {
 public:
  PermutationJobMachine() = default;
  explicit PermutationJobMachine(const std::vector<JobMachine> &permutation)
      : permutation_(permutation) {}

  std::vector<JobMachine> &permutation() {
    return permutation_;
  }

  Schedule ToSchedule(Situation situation) const override {
    Schedule schedule(situation);
    for (JobMachine jobMachine : permutation_) {
      schedule.AssignJob(std::get<1>(jobMachine), std::get<0>(jobMachine));
    }
    return schedule;
  }

 private:
  std::vector<JobMachine> permutation_;
};

}  // namespace genetic
}  // namespace lss

namespace std {

template<>
struct hash<tuple<lss::Job, lss::Machine>> {
  size_t operator()(const tuple<lss::Job, lss::Machine> &jm) const {
    size_t seed = hash<lss::Job>()(get<0>(jm));
    return  seed ^ (hash<lss::Machine>()(get<1>(jm)) + 0xCafeBabe + (seed << 6) + (seed >> 2));
  }
};

}  // namespace std

#endif  // LSS_GENETIC_PERMUTATION_CHROMOSOME_CHROMOSOME_H_

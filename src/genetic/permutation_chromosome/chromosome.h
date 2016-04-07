#ifndef LSS_GENETIC_PERMUTATION_CHROMOSOME_CHROMOSOME_H_
#define LSS_GENETIC_PERMUTATION_CHROMOSOME_CHROMOSOME_H_

#include <tuple>
#include <vector>

#include "gmock/gmock.h"

#include "genetic/algorithm.h"
#include "base/situation.h"

namespace lss {
namespace genetic {

using JobMachine = std::tuple<const lss::Job *, const lss::Machine *>;

class PermutationJobMachine : public Chromosome {
 public:
  PermutationJobMachine() = default;
  explicit PermutationJobMachine(const std::vector<JobMachine> &permutation)
      : permutation_(permutation) {}

  virtual std::vector<JobMachine> &permutation() {
    return permutation_;
  }

  Schedule ToSchedule() const override {
    return lss::Schedule();
  }

 private:
  std::vector<JobMachine> permutation_;
};

class PermutationJobMachineMock : public PermutationJobMachine {
 public:
  MOCK_METHOD0(permutation, std::vector<JobMachine> &());
};

}  // namespace genetic
}  // namespace lss

#endif  // LSS_GENETIC_PERMUTATION_CHROMOSOME_CHROMOSOME_H_

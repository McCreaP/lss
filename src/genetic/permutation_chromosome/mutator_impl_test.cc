#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "base/raw_situation.h"
#include "base/situation.h"
#include "genetic/permutation_chromosome/moves_impl.h"
#include "genetic/test_utils.h"

namespace lss {
namespace genetic {

using ::testing::Test;
using ::testing::NiceMock;
using ::testing::InvokeWithoutArgs;
using ::testing::Return;

std::vector<JobMachine> GetPermutation(std::vector<int> job_permutation,
                                       std::vector<int> machine_permutation,
                                       const Situation &situation) {
  std::vector<JobMachine> permutation;
  for (size_t i = 0; i < job_permutation.size(); ++i) {
    Job j = situation.jobs()[job_permutation[i]];
    Machine m = situation.machines()[machine_permutation[i]];
    JobMachine job_machine = std::make_tuple(j, m);
    permutation.push_back(job_machine);
  }
  return permutation;
}

class MutatorShould : public ::testing::Test {
 protected:
  void SetUp() {
    rand_ = std::make_shared<RandomMock>();
    raw_situation_ = GetSimpleRawSituation(kNumberOfJobs, kNumberOfMachines);
  }

  RawSituation raw_situation_;
  std::shared_ptr<RandomMock> rand_;
  const int kNumberOfJobs = 5;
  const int kNumberOfMachines = 3;
};

TEST_F(MutatorShould, take_job_machine_to_mutation_wrt_generated_random_number) {
  Situation situation(raw_situation_);
  auto permutation = GetPermutation({3, 1, 4, 0, 2}, {0, 1, 1, 0, 0}, situation);
  auto chromosome = PermutationJobMachine(permutation);
  std::vector<double> randoms = {0.1, 0.6, 0.4, 0.2, 0.6};
  Iterator<double> it(randoms);
  EXPECT_CALL(*rand_, GetRealInRange(0., 1.))
      .Times(kNumberOfJobs)
      .WillRepeatedly(InvokeWithoutArgs(&it, &Iterator<double>::Next));
  EXPECT_CALL(*rand_, Rand(3)).Times(3).WillRepeatedly(Return(2));

  MutatorImpl mutator(0.5, rand_);
  mutator.Mutate(situation, &chromosome);

  std::vector<int> expected_machines_order = {2, 1, 2, 2, 0};
  for (size_t i = 0; i < chromosome.permutation().size(); ++i) {
    EXPECT_EQ(expected_machines_order[i], (int)std::get<1>(chromosome.permutation()[i]).id());
  }
}

TEST_F(MutatorShould, select_new_machine_for_jobs_wrt_generated_random_number) {}

TEST_F(MutatorShould, gen_random_number_for_each_job_machine_pair) {}

}  // namespace genetic
}  // namespace lss
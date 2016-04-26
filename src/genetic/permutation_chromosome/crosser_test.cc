#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "genetic/permutation_chromosome/common.h"
#include "genetic/permutation_chromosome/moves_impl.h"
#include "genetic/test_utils.h"

namespace lss {
namespace genetic {

using ::testing::Return;

TEST(CrosserShould, cross) {
  const int kNumberOfJobs = 7;
  const int kNumberOfMachines = 3;
  auto rand = std::make_shared<RandomMock>();
  RawSituation rawSituation = GetSimpleRawSituation(kNumberOfJobs, kNumberOfMachines);
  Situation situation(rawSituation);

  auto permutation_1 = GetPermutation({0, 1, 2, 3, 4, 5, 6}, {0, 0, 0, 0, 0, 0, 0}, situation);
  auto permutation_2 = GetPermutation({6, 5, 4, 3, 2, 1, 0}, {2, 2, 2, 2, 2, 2, 2}, situation);
  auto chromosome_1 = PermutationJobMachine(permutation_1);
  auto chromosome_2 = PermutationJobMachine(permutation_2);

  EXPECT_CALL(*rand, Rand(kNumberOfJobs)).WillOnce(Return(2)).WillOnce(Return(6));
  CrosserImpl crosser(rand);
  crosser.Crossover(&chromosome_1, &chromosome_2);

  auto expected_permutation_1 =
      GetPermutation({6, 1, 2, 3, 4, 5, 0}, {2, 2, 0, 0, 0, 0, 2}, situation);
  auto expected_permutation_2 =
      GetPermutation({0, 5, 4, 3, 2, 1, 6}, {0, 0, 2, 2, 2, 2, 0}, situation);
  EXPECT_EQ(expected_permutation_1, chromosome_1.permutation());
  EXPECT_EQ(expected_permutation_2, chromosome_2.permutation());
}

TEST(CrosserShould, workWithEmptyChromosome) {
  const int kNumberOfJobs = 0;
  const int numberOfMachines = 3;
  RawSituation rawSituation = GetSimpleRawSituation(kNumberOfJobs, numberOfMachines);
  Situation situation(rawSituation);

  auto permutation_1 = GetPermutation({}, {}, situation);
  auto permutation_2 = GetPermutation({}, {}, situation);
  auto chromosome_1 = PermutationJobMachine(permutation_1);
  auto chromosome_2 = PermutationJobMachine(permutation_2);
  CrosserImpl crosser(std::make_shared<Random>());
  crosser.Crossover(&chromosome_1, &chromosome_2);

  EXPECT_EQ(permutation_1, chromosome_1.permutation());
  EXPECT_EQ(permutation_2, chromosome_2.permutation());
}

}  // namespace genetic
}  // namespace lss

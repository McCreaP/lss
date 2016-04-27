#include <memory>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "genetic/permutation_chromosome/moves_impl.h"
#include "base/random.h"
#include "base/situation.h"
#include "genetic/test_utils.h"

using ::testing::Test;

namespace lss {
namespace genetic {

class InitializerShould : public ::testing::Test {
 protected:
  void SetUp() {
    rawSituation_ = GetSimpleRawSituation(kNumberOfJobs, kNumberOfMachines);
    rand_ = std::make_shared<Random>();
  }

  RawSituation rawSituation_;
  std::shared_ptr<Random> rand_;
  const int kNumberOfJobs = 5;
  const int kNumberOfMachines = 3;
};

TEST_F(InitializerShould, return_population_with_requested_size) {
  Situation situation(rawSituation_);
  const int kRequestedPopulationSize = 42;

  InitializerImpl initializer(rand_);
  auto population = initializer.InitPopulation(situation, kRequestedPopulationSize);

  ASSERT_EQ(kRequestedPopulationSize, population.size());
}

TEST_F(InitializerShould, return_population_of_permutations) {
  Situation situation(rawSituation_);
  const int kPopulationSize = 42;

  InitializerImpl initializer(rand_);
  auto population = initializer.InitPopulation(situation, kPopulationSize);

  for (PermutationJobMachine &chromosome : population) {
    ASSERT_EQ(situation.jobs().size(), chromosome.permutation().size());
    for (const Job &job : situation.jobs()) {
      bool found_job_in_permutation = false;
      for (const JobMachine &jm : chromosome.permutation()) {
        found_job_in_permutation |= std::get<0>(jm) == job;
      }
      ASSERT_TRUE(found_job_in_permutation);
    }
  }
}

}  // namespace genetic
}  // namespace lss

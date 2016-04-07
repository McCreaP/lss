#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "genetic/mocks.h"
#include "genetic/permutation_chromosome/test_utils.h"

using ::testing::Test;

namespace lss {
namespace genetic {

class InitializerShould : public ::testing::Test {
 protected:
  void SetUp() {
    rawSituation_ = GetSimpleRawSituation(kNumberOfJobs, kNumberOfMachines);
    rand_ = std::make_shared<RandomMock>();
  }

  RawSituation rawSituation_;
  std::shared_ptr<RandomMock> rand_;
  const int kNumberOfJobs = 5;
  const int kNumberOfMachines = 3;
};

TEST_F(InitializerShould, return_population_with_requested_size) {}

}  // namespace genetic
}  // namespace lss
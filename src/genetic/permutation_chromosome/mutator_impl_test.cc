#include <base/raw_situation.h>
#include <base/situation.h>
#include <genetic/mocks.h>
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "moves_impl.h"

namespace lss {
namespace genetic {

using ::testing::Test;
using ::testing::NiceMock;
using ::testing::InvokeWithoutArgs;

class MutatorShould : public ::testing::Test {
 protected:
  void SetUp() {
    auto jobs = InitJobs();
    auto machines = InitMachines();
    auto machine_set = InitMachineSet();

    rawSituation_ = {0, {}, {}, {}, {}, {}, {}, {}};
    rawSituation_.jobs_ = std::move(jobs);
    rawSituation_.machines_ = std::move(machines);
    rawSituation_.add(machine_set);

    rand_ = std::make_shared<RandomMock>();
  }

  std::vector<RawJob> InitJobs() {
    std::vector<RawJob> jobs;
    for (int id = 0; id < kNumberOfJobs; ++id) {
      RawJob job = {0, 0, 0, 0, 0, 0, Context(0, 0, 0)};
      job.id(id).machine_set(kMachineSetId);
      jobs.push_back(job);
    }
    return jobs;
  }

  std::vector<RawMachine> InitMachines() {
    std::vector<RawMachine> machines;
    for (int id = 0; id < kNumberOfMachines; ++id) {
      RawMachine machine = {0, MachineState::kIdle, Context(0, 0, 0)};
      machine.id(id);
      machines.push_back(machine);
    }
    return machines;
  }

  RawMachineSet InitMachineSet() {
    RawMachineSet machineSet = {0, {}};
    machineSet.id(kMachineSetId);
    for (int id = 0; id < kNumberOfMachines; ++id) {
      machineSet.add(id);
    }
    return machineSet;
  };

  RawSituation rawSituation_;
  std::shared_ptr<RandomMock> rand_;
  const int kNumberOfJobs = 5;
  const int kNumberOfMachines = 3;
  const int kMachineSetId = 42;
};

TEST_F(MutatorShould, take_job_machine_to_mutation_wrt_generated_random_number) {
  Situation situation(rawSituation_, false);
  PermutationJobMachineMock chromosome;

  std::vector<double> randoms = {0.1, 0.6, 0.4, 0.2, 0.6};
  Iterator<double> it(randoms);
  EXPECT_CALL(*rand_, GetRealInRange(0., 1.))
      .Times(kNumberOfJobs)
      .WillRepeatedly(InvokeWithoutArgs(&it, &Iterator<double>::Next));

  MutatorImpl mutator(0.5, rand_);
  mutator.Mutate(situation, &chromosome);

}

TEST_F(MutatorShould, select_new_machine_for_jobs_wrt_generated_random_number) {}

TEST_F(MutatorShould, gen_random_number_for_each_job_machine_pair) {}

}  // namespace genetic
}  // namespace lss
#ifndef LSS_GENETIC_PERMUTATION_CHROMOSOME_TEST_UTILS_H_
#define LSS_GENETIC_PERMUTATION_CHROMOSOME_TEST_UTILS_H_

#include "base/raw_situation.h"
#include "genetic/test_utils.h"

namespace lss {
namespace genetic {
namespace {

RawBatch InitBatch(int id, int accountId) {
  RawBatch batch = {0, 0, 0, 0, 0, 0, 0, 0};
  return batch.id(id).account(accountId);
}

RawAccount InitAccount(int id) {
  RawAccount acc = {id, 0};
  return acc;
}

std::vector<RawChangeCost> InitChangeCosts() {
  std::vector<RawChangeCost> changeCosts;
  for (int i = 0; i < 8; ++i) {
    RawChangeCost changeCost = {Change(i & 1, i & 2, i & 4), 0};
    changeCosts.push_back(changeCost);
  }
  return changeCosts;
}

std::vector<RawJob> InitJobs(int kNumberOfJobs, int batchId, int kMachineSetId) {
  std::vector<RawJob> jobs;
  for (int id = 0; id < kNumberOfJobs; ++id) {
    RawJob job = {0, 0, 0, 0, 0, 0, Context(0, 0, 0)};
    job.id(id)
        .batch(batchId)
        .machine(0)
        .machine_set(kMachineSetId);
    jobs.push_back(job);
  }
  return jobs;
}

std::vector<RawMachine> InitMachines(int kNumberOfMachines) {
  std::vector<RawMachine> machines;
  for (int id = 0; id < kNumberOfMachines; ++id) {
    RawMachine machine = {id, MachineState::kIdle, Context(0, 0, 0)};
    machines.push_back(machine);
  }
  return machines;
}

RawMachineSet InitMachineSet(int kMachineSetId, int kNumberOfMachines) {
  RawMachineSet machineSet = {0, {}};
  machineSet.id(kMachineSetId);
  for (int id = 0; id < kNumberOfMachines; ++id) {
    machineSet.add(id);
  }
  return machineSet;
}

}  // namespace

template<class T>
Iterator<T>::Iterator(std::vector<T> v) : v_(std::move(v)) {}

template<class T>
T Iterator<T>::Next() {
  return v_[counter_++];
}

RawSituation GetSimpleRawSituation(int numberOfJobs, int numberOfMachines) {
  const int batchId = 1;
  const int accountId = 1;
  const int machineSetId = 1;
  auto batch = InitBatch(batchId, accountId);
  auto account = InitAccount(accountId);
  auto changeCosts = InitChangeCosts();
  auto jobs = InitJobs(numberOfJobs, batchId, machineSetId);
  auto machines = InitMachines(numberOfMachines);
  auto machine_set = InitMachineSet(machineSetId, numberOfMachines);

  RawSituation rawSituation = {0, {}, {}, {}, {}, {}, {}, {}};
  rawSituation.jobs_ = std::move(jobs);
  rawSituation.machines_ = std::move(machines);
  rawSituation.change_costs_ = std::move(changeCosts);
  rawSituation.add(batch)
      .add(machine_set)
      .add(account);

  return rawSituation;
}

}  // namespace genetic
}  // namespace lss

#endif  // LSS_GENETIC_PERMUTATION_CHROMOSOME_TEST_UTILS_H_

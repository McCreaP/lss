#ifndef LSS_GENETIC_PERMUTATION_CHROMOSOME_TEST_UTILS_H_
#define LSS_GENETIC_PERMUTATION_CHROMOSOME_TEST_UTILS_H_

#include "base/raw_situation.h"
#include "genetic/test_utils.h"

namespace lss {
namespace genetic {
namespace {

RawBatch InitBatch(int id, int accountId) {
  RawBatch batch;
  return batch.id(id).account(accountId);
}

RawAccount InitAccount(int id) {
  RawAccount acc;
  return acc.id(id);
}

std::vector<RawChangeCost> InitChangeCosts() {
  std::vector<RawChangeCost> changeCosts;
  for (int i = 0; i < 8; ++i) {
    RawChangeCost changeCost;
    changeCost.change(Change(i & 1, i & 2, i & 4));
    changeCosts.push_back(changeCost);
  }
  return changeCosts;
}

std::vector<RawJob> InitJobs(int kNumberOfJobs, int batchId, int kMachineSetId) {
  std::vector<RawJob> jobs;
  for (int id = 0; id < kNumberOfJobs; ++id) {
    RawJob job;
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
    RawMachine machine;
    machines.push_back(machine.id(id));
  }
  return machines;
}

RawMachineSet InitMachineSet(int kMachineSetId, int kNumberOfMachines) {
  RawMachineSet machineSet;
  machineSet.id(kMachineSetId);
  for (int id = 0; id < kNumberOfMachines; ++id) {
    machineSet.add(id);
  }
  return machineSet;
}

}  // namespace

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

  RawSituation rawSituation;
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

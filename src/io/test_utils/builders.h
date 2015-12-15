#ifndef LSS_IO_TEST_UTILS_BUILDERS_H_
#define LSS_IO_TEST_UTILS_BUILDERS_H_

#include <cassert>
#include <vector>

#include "io/raw_input_types.h"

namespace lss {
namespace io {

class RawDataBuilder {
 public:
  RawDataBuilder& WithBatch(const Batch& batch) {
    raw_data_.batches.push_back(batch);
    return *this;
  }

  RawDataBuilder& WithBatches(const std::vector <Batch>& batches) {
    for (const Batch& batch : batches)
      raw_data_.batches.push_back(batch);
    return *this;
  }

  RawDataBuilder& WithJob(const Job& job) {
    raw_data_.jobs.push_back(job);
    return *this;
  }

  RawDataBuilder& WithJobs(const std::vector <Job>& jobs) {
    for (const Job& job : jobs)
      raw_data_.jobs.push_back(job);
    return *this;
  }

  RawDataBuilder& WithMachine(const Machine& machine) {
    raw_data_.machines.push_back(machine);
    return *this;
  }

  RawDataBuilder& WithMachines(const std::vector<Machine>& machines) {
    for (const Machine& machine : machines)
      raw_data_.machines.push_back(machine);
    return *this;
  }

  RawDataBuilder& WithMachineSet(const MachineSet& machine_set) {
    raw_data_.machine_sets.push_back(machine_set);
    return *this;
  }

  RawDataBuilder& WithMachineSets(const std::vector<MachineSet>& machine_sets) {
    for (const MachineSet& machine_set : machine_sets)
      raw_data_.machine_sets.push_back(machine_set);
    return *this;
  }

  RawData Build() {
    return raw_data_;
  }

 private:
  RawData raw_data_;
};

class BatchBuilder {
 public:
  BatchBuilder() {
    batch_ = {1, 1, 1.0, 1.0, 1.0, 1.0};
  }

  BatchBuilder& WithId(int id) {
    batch_.id = id;
    return *this;
  }

  BatchBuilder& WithTimelyReward(double timely_reward) {
    batch_.timely_reward = timely_reward;
    return *this;
  }

  BatchBuilder& WithReward(double reward) {
    batch_.reward = reward;
    return *this;
  }

  BatchBuilder& WithExpectedTime(double expected_time) {
    batch_.expected_time = expected_time;
    return *this;
  }

  BatchBuilder& WithDue(double due) {
    batch_.due = due;
    return *this;
  }

  Batch Build() {
    return batch_;
  }

 private:
  Batch batch_;
};

class JobBuilder {
 public:
  JobBuilder() {
    job_ = {1, 1, 1.0, 1, 1, 1, 1};
  }

  JobBuilder& WithId(int id) {
    job_.id = id;
    return *this;
  }

  JobBuilder& WithBatchId(int batch_id) {
    job_.batch_id = batch_id;
    return *this;
  }

  JobBuilder& WithDuration(double duration) {
    job_.duration = duration;
    return *this;
  }

  JobBuilder& WithContext(const std::vector<int>& context) {
    assert(context.size() == kContextN);
    for (int i = 0; i < kContextN; ++i)
      job_.context[i] = context[i];
    return *this;
  }

  Job Build() {
    return job_;
  }

 private:
  Job job_;
};

}  // namespace io
}  // namespace lss

#endif  // LSS_IO_TEST_UTILS_BUILDERS_H_

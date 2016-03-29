// This file defines simple structs to store information about the world. Most fields directly
// correspond with data available in the input file; a few which don't are commented appropriately.
#ifndef LSS_BASE_RAW_SITUATION_H_
#define LSS_BASE_RAW_SITUATION_H_

#include <iostream>
#include <vector>

#include "base/types.h"

namespace lss {

struct RawJob {
  IdType id;
  IdType batch_id;
  IdType machine_id;  // The machine this job is assigned to; not present in the input file.
  Duration duration;  // Expected duration barring setup time.
  Time start_time;    // Not present in the input file.
  IdType machineset_id;
  int context[Context::kSize];

  bool operator==(const RawJob& rhs) const;
};

// The following fields are not named in objective function definition:
// - timely_reward (called A in paper),
// - reward (called B in paper),
// - expected_time (called T in paper).
struct RawBatch {
  IdType id;
  IdType account_id;
  FloatType job_reward;
  FloatType job_timely_reward;
  FloatType reward;
  FloatType timely_reward;
  Duration expected_time;
  Time due;

  bool operator==(const RawBatch& rhs) const;
};

struct RawMachine {
  IdType id;
  MachineState state;
  int context[Context::kSize];  // Not present in the input file.
};

struct RawMachineSet {
  IdType id;
  std::vector<IdType> machines;
};

struct RawAccount {
  IdType id;
  FloatType alloc;
};

struct RawContextChange {
  bool changed[Context::kSize];
  Cost cost;
};

struct RawSituation {
  Time time_stamp;  // Not present in the input file.
  std::vector<RawMachine> machines;
  std::vector<RawMachineSet> machine_sets;
  std::vector<RawMachineSet> fair_sets;
  std::vector<RawJob> jobs;
  std::vector<RawBatch> batches;
  std::vector<RawAccount> accounts;
  std::vector<RawContextChange> context_changes;
};

}  // namespace lss

#endif  // LSS_BASE_RAW_SITUATION_H_

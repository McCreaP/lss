// This file defines simple structs to store information about the world. Most fields directly
// correspond with data available in the input file; a few which don't are commented appropriately.
#ifndef LSS_BASE_RAW_SITUATION_H_
#define LSS_BASE_RAW_SITUATION_H_

#include "base/types.h"

#include <iostream>
#include <vector>

namespace lss {

struct RawJob {
  int id;
  int batch_id;
  int machine_id;    // The machine this job is assigned to; not present in the input file.
  double duration;   // Expected duration barring setup time.
  double start_time; // Not present in the input file.
  int machineset_id;
  int context[Context::kSize];

  bool operator==(const RawJob& rhs) const;
};

// The following fields are not named in objective function definition:
// - timely_reward (called A in paper),
// - reward (called B in paper),
// - expected_time (called T in paper).
struct RawBatch {
  int id;
  int account_id;
  double timely_reward;
  double reward;
  double expected_time;
  double due;

  bool operator==(const RawBatch& rhs) const;
};

struct RawMachine {
  int id;
  MachineState state;
  int context[Context::kSize];  // Not present in the input file.
};

struct RawMachineSet {
  int id;
  std::vector<int> machines;
};

struct RawAccount {
  int id;
  double alloc;
};

struct RawContextChange {
  bool changed[Context::kSize];
  int cost;
};

struct RawSituation {
  double time_stamp;  // Not present in the input file.
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

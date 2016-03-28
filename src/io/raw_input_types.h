// This file defines types which represent input data exactly
// as it is read from files.
#ifndef LSS_IO_RAW_INPUT_TYPES_H_
#define LSS_IO_RAW_INPUT_TYPES_H_

#include <iostream>
#include <vector>

namespace lss {
namespace io {

static constexpr int kContextN = 3;

enum class MachineState {
  kIdle = 0,
  kWorking = 1,
  kDead = 2,
};

struct RawJob {
  int id;
  int batch_id;
  double duration;  // Expected duration barring setup time.
  int machineset_id;
  int context[kContextN];

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
  bool changed[kContextN];
  int cost;
};

struct RawSituation {
  std::vector<RawMachine> machines;
  std::vector<RawMachineSet> machine_sets;
  std::vector<RawMachineSet> fair_machine_sets;
  std::vector<RawJob> jobs;
  std::vector<RawBatch> batches;
  std::vector<RawAccount> accounts;
  std::vector<RawContextChange> context_changes;
};

}  // namespace io
}  // namespace lss

#endif  // LSS_IO_RAW_INPUT_TYPES_H_

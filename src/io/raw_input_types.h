// This file defines types which represent input data exactly
// as it is read from files.
#ifndef LSS_IO_RAW_INPUT_TYPES_H_
#define LSS_IO_RAW_INPUT_TYPES_H_

#include <vector>

namespace lss {
namespace io {

static constexpr int kContextN = 3;

enum class MachineState {
  kIdle = 0,
  kWorking = 1,
  kDead = 2,
};

struct Job {
  int id;
  int batch_id;
  double duration;  // Expected duration barring setup time.
  int machineset_id;
  int context[kContextN];

  Job& operator=(const Job& rhs) {
    id = rhs.id;
    machineset_id = rhs.machineset_id;
    batch_id = rhs.batch_id;
    duration = rhs.duration;
    for (int i = 0; i < kContextN; ++i)
      context[i] = rhs.context[i];
    return *this;
  }
};

// The following fields are not named in objective function definition:
// - timely_reward (called A in paper),
// - reward (called B in paper),
// - expected_time (called T in paper).
struct Batch {
  int id;
  int account_id;
  double timely_reward;
  double reward;
  double expected_time;
  double due;
};

struct Machine {
  int id;
  MachineState state;
};

struct MachineSet {
  int id;
  std::vector<int> machines;
};

struct Account {
  int id;
  double alloc;
};

struct ContextChange {
  bool changed[kContextN];
  int cost;
};

struct RawData {
  std::vector<Machine> machines;
  std::vector<MachineSet> machine_sets;
  std::vector<MachineSet> fair_machine_sets;
  std::vector<Job> jobs;
  std::vector<Batch> batches;
  std::vector<Account> accounts;
  std::vector<ContextChange> context_changes;
};

}  // namespace io
}  // namespace lss

#endif  // LSS_IO_RAW_INPUT_TYPES_H_

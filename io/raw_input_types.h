// This file defines types which represent input data exactly
// as it is read from files.
#ifndef LSS_IO_RAW_INPUT_TYPES_H_
#define LSS_IO_RAW_INPUT_TYPES_H_

#include <vector>

namespace lss {
namespace io {

enum class MachineState {
  kIdle = 0,
  kWorking = 1,
  kDead = 2,
};

struct Job {
  int id;
  int batch_id;
  int account_id;
  int duration;
  int machineset_id;
  int context[3];
};

// TODO: Possibly find better names for fields: timely_reward, reward,
// expected_time. They are called A, B, T respectively in objective funciton
// definition.
struct Batch {
  int id;
  int timely_reward;
  int reward;
  int expected_time;
  int due;
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

// TODO: It is very unclear what contexts are and thus it is difficult to say
// what these fields mean and if their names are suitable.
struct ContextChange {
  int context_changed[3];
  int switch_cost;
};

}  // namespace io
}  // namespace lss

#endif  // LSS_IO_RAW_INPUT_TYPES_H_

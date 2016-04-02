// This file defines simple structs to store information about the world. Most fields directly
// correspond with data available in the input file; a few which don't are commented appropriately.
#ifndef LSS_BASE_RAW_SITUATION_H_
#define LSS_BASE_RAW_SITUATION_H_

#include <iostream>
#include <vector>

#include "base/types.h"

namespace lss {

struct RawJob {
  IdType id_;
  IdType batch_;
  IdType machine_;  // The machine this job is assigned to; not present in the input file.
  Duration duration_;  // Expected duration barring setup time.
  Time start_time_;    // Not present in the input file.
  IdType machine_set_;
  Context context_;

  bool operator==(const RawJob& rhs) const;
};

// The following fields are not named in objective function definition:
// - timely_reward_ (called A in paper),
// - reward_ (called B in paper),
// - duration_ (called T in paper).
struct RawBatch {
  IdType id_;
  IdType account_;
  FloatType job_reward_;
  FloatType job_timely_reward_;
  FloatType reward_;
  FloatType timely_reward_;
  Duration duration_;
  Time due_;

  bool operator==(const RawBatch& rhs) const;
};

struct RawMachine {
  IdType id_;
  MachineState state_;
  Context context_;  // Not present in the input file.
};

struct RawMachineSet {
  IdType id_;
  std::vector<IdType> machines_;
};

struct RawAccount {
  IdType id_;
  FloatType alloc_;
};

struct RawChangeCost {
  Change change_;
  Cost cost_;
};

struct RawSituation {
  Time time_stamp_;  // Not present in the input file.
  std::vector<RawMachine> machines_;
  std::vector<RawMachineSet> machine_sets_;
  std::vector<RawMachineSet> fair_sets_;
  std::vector<RawJob> jobs_;
  std::vector<RawBatch> batches_;
  std::vector<RawAccount> accounts_;
  std::vector<RawChangeCost> change_costs_;
};

}  // namespace lss

#endif  // LSS_BASE_RAW_SITUATION_H_

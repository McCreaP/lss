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
  IdType machine_;     // The machine this job is assigned to; not present in the input file.
  Duration duration_;  // Expected duration barring setup time.
  Time start_time_;    // Not present in the input file.
  IdType machine_set_;
  Context context_;

  RawJob& id(IdType val) { id_ = val; return *this; }
  RawJob& batch(IdType val) { batch_ = val; return *this; }
  RawJob& machine(IdType val) { machine_ = val; return *this; }
  RawJob& duration(Duration val) { duration_ = val; return *this; }
  RawJob& start_time(Time val) { start_time_ = val; return *this; }
  RawJob& machine_set(IdType val) { machine_set_ = val; return *this; }
  RawJob& context(Context val) { context_ = val; return *this; }

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

  RawBatch& id(IdType val) { id_ = val; return *this; }
  RawBatch& account(IdType val) { account_ = val; return *this; }
  RawBatch& job_reward(FloatType val) { job_reward_ = val; return *this; }
  RawBatch& job_timely_reward(FloatType val) { job_timely_reward_ = val; return *this; }
  RawBatch& reward(FloatType val) { reward_ = val; return *this; }
  RawBatch& timely_reward(FloatType val) { timely_reward_ = val; return *this; }
  RawBatch& duration(Duration val) { duration_ = val; return *this; }
  RawBatch& due(Time val) { due_ = val; return *this; }

  bool operator==(const RawBatch& rhs) const;
};

struct RawMachine {
  IdType id_;
  MachineState state_;
  Context context_;  // Not present in the input file.

  RawMachine& id(IdType val) { id_ = val; return *this; }
  RawMachine& state(MachineState val) { state_ = val; return *this; }
  RawMachine& context(Context val) { context_ = val; return *this; }
};

struct RawMachineSet {
  IdType id_;
  std::vector<IdType> machines_;

  RawMachineSet& id(IdType val) { id_ = val; return *this; }
  RawMachineSet& add(IdType val) { machines_.push_back(val); return *this; };
};

struct RawFairSet {
  IdType id_;
  std::vector<IdType> machines_;

  RawFairSet& id(IdType val) { id_ = val; return *this; }
  RawFairSet& add(IdType val) { machines_.push_back(val); return *this; };
};

struct RawAccount {
  IdType id_;
  FloatType alloc_;

  RawAccount& id(IdType val) { id_ = val; return *this; }
  RawAccount& alloc(FloatType val) { alloc_ = val; return *this; }
};

struct RawChangeCost {
  Change change_;
  Cost cost_;

  RawChangeCost& change(Change val) { change_ = val; return *this; }
  RawChangeCost& cost(Cost val) { cost_ = val; return *this; }
};

struct RawSituation {
  Time time_stamp_;  // Not present in the input file.
  std::vector<RawMachine> machines_;
  std::vector<RawMachineSet> machine_sets_;
  std::vector<RawFairSet> fair_sets_;
  std::vector<RawJob> jobs_;
  std::vector<RawBatch> batches_;
  std::vector<RawAccount> accounts_;
  std::vector<RawChangeCost> change_costs_;
};

}  // namespace lss

#endif  // LSS_BASE_RAW_SITUATION_H_

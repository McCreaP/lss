#ifndef LSS_EXPERIMENTAL_SITUATION_H_
#define LSS_EXPERIMENTAL_SITUATION_H_

#include "experimental/basic_types.h"

#include <vector>

namespace lss {

class Machine;
class MachineSet;
class FairSet;
class Account;
class Batch;
class Job;
class ChangeCosts;

class Machine {
 public:
  using MachineSets = const std::vector<MachineSet>&;

  Machine() = default;

  Id<Machine> id() const;
  MachineState state() const;
  MachineSets machine_sets() const;
  FairSet fair_set() const;
  Job job() const;

 private:
  struct Data;
  Machine(Data *data) : data_(data) {}
  const Data *const data_ = nullptr;
  friend class Situation;
};

class MachineSet {
 public:
  using Machines = const std::vector<Machine>&;

  MachineSet() = default;

  Id<MachineSet> id() const;
  Machines machines() const;

 private:
  struct Data;
  MachineSet(Data *data) : data_(data) {}
  const Data *const data_ = nullptr;
  friend class Situation;
};

class FairSet {
 public:
  using Machines = const std::vector<Machine>&;

  FairSet() = default;

  Id<FairSet> id() const;
  Machines machines() const;

 private:
  struct Data;
  FairSet(Data *data) : data_(data) {}
  const Data *const data_ = nullptr;
  friend class Situation;
};

class Account {
 public:
  using Batches = const std::vector<Batch>&;

  Account() = default;

  Id<Account> id() const;
  FloatType alloc() const;
  Batches batches() const;

 private:
  struct Data;
  Account(Data *data) : data_(data) {}
  const Data *const data_ = nullptr;
  friend class Situation;
};

class Batch {
 public:
  using Jobs = const std::vector<Job>&;

  Batch() = default;

  Id<Batch> id() const;
  FloatType reward() const;
  FloatType timely_reward() const;
  FloatType job_reward() const;
  FloatType job_timely_reward() const;
  Duration duration() const;
  Time due_time() const;
  Account account() const;
  Jobs jobs() const;

 private:
  struct Data;
  Batch(Data *data) : data_(data) {}
  const Data *const data_ = nullptr;
  friend class Situation;
};

class Job {
 public:
  Job() = default;

  Id<Job> id() const;
  Time start_time() const;
  Duration duration() const;
  Context context() const;
  Machine machine() const;
  MachineSet machine_set() const;
  Batch batch() const;

 private:
  struct Data;
  Job(Data *data) : data_(data) {}
  const Data *const data_ = nullptr;
  friend class Situation;
};

class ChangeCosts {
 public:
  Cost cost(Change c) const;
  Cost cost(Context from, Context to) const;
};

class Situation {
 public:
  using Machines = const std::vector<Machine>&;
  using MachineSets = const std::vector<MachineSet>&;
  using FairSets = const std::vector<FairSet>&;
  using Accounts = const std::vector<Account>&;
  using Batches = const std::vector<Batch>&;
  using Jobs = const std::vector<Job>&;

  ~Situation();

  Jobs jobs() const { return jobs_; }
  Machines machines() const { return machines_; }
  MachineSets machine_sets() const { return machine_sets_; }
  FairSets fair_sets() const { return fair_sets_; }
  Accounts accounts() const { return accounts_; }
  Batches batches() const { return batches_; }

 private:
  std::vector<Machine> machines_;
  std::vector<MachineSet> machine_sets_;
  std::vector<FairSet> fair_sets_;
  std::vector<Account> accounts_;
  std::vector<Batch> batches_;
  std::vector<Job> jobs_;
};

struct Machine::Data {
  Id<Machine> id;

  MachineState state;

  std::vector<MachineSet> machine_sets;
  FairSet fair_set;
  Job job;
};

struct MachineSet::Data {
  Id<MachineSet> id;

  std::vector<Machine> machines;
};

struct FairSet::Data {
  Id<FairSet> id;

  std::vector<Machine> machines;
};

struct Account::Data {
  Id<Account> id;

  FloatType alloc;

  std::vector<Batch> batches;
};

struct Batch::Data {
  Id<Batch> id;

  FloatType reward;
  FloatType timely_reward;
  FloatType job_reward;
  FloatType job_timely_reward;
  Duration duration;
  Time due_time;

  Account account;
  std::vector<Job> jobs;
};

struct Job::Data {
  Id<Job> id;

  Time start_time;
  Duration duration;
  Context context;

  Machine machine;
  MachineSet machine_set;
  Batch batch;
};

}  // namespace lss

#endif  // LSS_EXPERIMENTAL_SITUATION_H_

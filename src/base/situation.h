#ifndef LSS_BASE_SITUATION_H_
#define LSS_BASE_SITUATION_H_

#include <vector>

#include "base/raw_situation.h"
#include "base/types.h"

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

  explicit operator bool() { return data_; }

  Id<Machine> id() const;
  MachineState state() const;
  Context context() const;
  MachineSets machine_sets() const;
  FairSet fair_set() const;
  Job job() const;

 private:
  struct Data;
  explicit Machine(Data *data) : data_(data) {}
  Data *data_ = nullptr;
  friend class Situation;
};

class MachineSet {
 public:
  using Machines = const std::vector<Machine>&;

  MachineSet() = default;

  explicit operator bool() { return data_; }

  Id<MachineSet> id() const;
  Machines machines() const;

 private:
  struct Data;
  explicit MachineSet(Data *data) : data_(data) {}
  Data *data_ = nullptr;
  friend class Situation;
};

class FairSet {
 public:
  using Machines = const std::vector<Machine>&;

  FairSet() = default;

  explicit operator bool() { return data_; }

  Id<FairSet> id() const;
  Machines machines() const;

 private:
  struct Data;
  explicit FairSet(Data *data) : data_(data) {}
  Data *data_ = nullptr;
  friend class Situation;
};

class Account {
 public:
  using Batches = const std::vector<Batch>&;

  Account() = default;

  explicit operator bool() { return data_; }

  Id<Account> id() const;
  FloatType alloc() const;
  Batches batches() const;

 private:
  struct Data;
  explicit Account(Data *data) : data_(data) {}
  Data *data_ = nullptr;
  friend class Situation;
};

class Batch {
 public:
  using Jobs = const std::vector<Job>&;

  Batch() = default;

  explicit operator bool() { return data_; }

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
  explicit Batch(Data *data) : data_(data) {}
  Data *data_ = nullptr;
  friend class Situation;
};

class Job {
 public:
  Job() = default;

  explicit operator bool() { return data_; }

  Id<Job> id() const;
  Duration duration() const;
  Context context() const;
  Time start_time() const;
  Machine machine() const;
  MachineSet machine_set() const;
  Batch batch() const;

 private:
  struct Data;
  explicit Job(Data *data) : data_(data) {}
  Data *data_ = nullptr;
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

  // The behavior is undefined if raw contains several objects of the same type with the same ID.
  // Situation(const RawSituation &raw);

  ~Situation();

  Time time_stamp() const { return time_stamp_; }

  Machine operator[](Id<Machine> id) const { return Get(machines_, id); }
  MachineSet operator[](Id<MachineSet> id) const { return Get(machine_sets_, id); }
  FairSet operator[](Id<FairSet> id) const { return Get(fair_sets_, id); }
  Account operator[](Id<Account> id) const { return Get(accounts_, id); }
  Batch operator[](Id<Batch> id) const { return Get(batches_, id); }
  Job operator[](Id<Job> id) const { return Get(jobs_, id); }

  Machines machines() const { return machines_; }
  MachineSets machine_sets() const { return machine_sets_; }
  FairSets fair_sets() const { return fair_sets_; }
  Accounts accounts() const { return accounts_; }
  Batches batches() const { return batches_; }
  Jobs jobs() const { return jobs_; }

 private:
  Time time_stamp_;

  std::vector<Machine> machines_;
  std::vector<MachineSet> machine_sets_;
  std::vector<FairSet> fair_sets_;
  std::vector<Account> accounts_;
  std::vector<Batch> batches_;
  std::vector<Job> jobs_;

  template<class T>
  static T Get(const std::vector<T> &from, Id<T> id);
};

struct Machine::Data {
  Id<Machine> id;

  MachineState state;
  Context context;

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

  Duration duration;
  Context context;
  Time start_time;

  Machine machine;
  MachineSet machine_set;
  Batch batch;
};

inline Id<Machine> Machine::id() const { return data_->id; }
inline MachineState Machine::state() const { return data_->state; }
inline Context Machine::context() const {return data_->context; }
inline Machine::MachineSets Machine::machine_sets() const { return data_->machine_sets; }
inline FairSet Machine::fair_set() const { return data_->fair_set; }
inline Job Machine::job() const { return data_->job; }

inline Id<MachineSet> MachineSet::id() const { return data_->id; }
inline MachineSet::Machines MachineSet::machines() const { return data_->machines; }

inline Id<FairSet> FairSet::id() const { return data_->id; }
inline FairSet::Machines FairSet::machines() const { return data_->machines; }

inline Id<Account> Account::id() const { return data_->id; }
inline FloatType Account::alloc() const { return data_->alloc; }
inline Account::Batches Account::batches() const { return data_->batches; }

inline Id<Batch> Batch::id() const { return data_->id; }
inline FloatType Batch::reward() const { return data_->reward; }
inline FloatType Batch::timely_reward() const { return data_->timely_reward; }
inline FloatType Batch::job_reward() const { return data_->job_reward; }
inline FloatType Batch::job_timely_reward() const { return data_->job_timely_reward; }
inline Duration Batch::duration() const { return data_->duration; }
inline Time Batch::due_time() const { return data_->due_time; }
inline Account Batch::account() const { return data_->account; }
inline Batch::Jobs Batch::jobs() const { return data_->jobs; }

inline Id<Job> Job::id() const { return data_->id; }
inline Time Job::start_time() const { return data_->start_time; }
inline Duration Job::duration() const { return data_->duration; }
inline Context Job::context() const { return data_->context; }
inline Machine Job::machine() const { return data_->machine; }
inline MachineSet Job::machine_set() const { return data_->machine_set; }
inline Batch Job::batch() const { return data_->batch; }

}  // namespace lss

#endif  // LSS_BASE_SITUATION_H_

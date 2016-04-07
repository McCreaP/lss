// This header provides Situation class - a convenient representation of input and some
// additional information about program state. General note: all accessors which return
// multiple objects return a vector (but you should not rely on the actual type, please
// use appropriate typedefs instead) where objects are sorted in ascending order in regard
// to their IDs.

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
  explicit operator bool() { return data_ != nullptr; }

  Id<Machine> id() const;            // Property
  MachineState state() const;        // Property
  Context context() const;           // Property; extra
  MachineSets machine_sets() const;  // Backward relation
  FairSet fair_set() const;          // Backward relation
  Job job() const;                   // Backward relation; extra

 private:
  struct Data;
  explicit Machine(Data *data) : data_(data) {}
  Data *data_ = nullptr;
  friend class Situation;
};

class MachineSet {
 public:
  using Machines = const std::vector<Machine>&;
  using Jobs = const std::vector<Job>&;

  MachineSet() = default;
  explicit operator bool() { return data_ != nullptr; }

  Id<MachineSet> id() const;  // Property
  Machines machines() const;  // Forward relation
  Jobs jobs() const;          // Backward relation

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
  explicit operator bool() { return data_ != nullptr; }

  Id<FairSet> id() const;     // Property
  Machines machines() const;  // Forward relation

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
  explicit operator bool() { return data_ != nullptr; }

  Id<Account> id() const;   // Property
  FloatType alloc() const;  // Property
  Batches batches() const;  // Backward relation

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
  explicit operator bool() { return data_ != nullptr; }

  Id<Batch> id() const;                 // Property
  FloatType job_reward() const;         // Property
  FloatType job_timely_reward() const;  // Property
  FloatType reward() const;             // Property
  FloatType timely_reward() const;      // Property
  Duration duration() const;            // Property
  Time due() const;                     // Property
  Account account() const;              // Forward relation
  Jobs jobs() const;                    // Backward relation

 private:
  struct Data;
  explicit Batch(Data *data) : data_(data) {}
  Data *data_ = nullptr;
  friend class Situation;
};

class Job {
 public:
  Job() = default;
  explicit operator bool() { return data_ != nullptr; }

  Id<Job> id() const;              // Property
  Duration duration() const;       // Property
  Context context() const;         // Property
  Time start_time() const;         // Property; extra
  // TODO(kzyla): Make this relation optional.
  Machine machine() const;         // Forward relation; extra
  MachineSet machine_set() const;  // Forward relation
  Batch batch() const;             // Forward relation

 private:
  struct Data;
  explicit Job(Data *data) : data_(data) {}
  Data *data_ = nullptr;
  friend class Situation;
};

class ChangeCosts {
 public:
  Cost cost(Change c) const { return cost_[static_cast<size_t>(c)]; }
  Cost cost(Context from, Context to) const { return cost(Change(from, to)); }

 private:
  ChangeCosts(const ChangeCosts &) = delete;
  ChangeCosts& operator=(const ChangeCosts &) = delete;

  ChangeCosts(const std::vector<RawChangeCost> &raw, bool safe);

  Cost cost_[Change::kNum] = {};

  friend class Situation;
};

class Situation {
 public:
  using Machines = const std::vector<Machine>&;
  using MachineSets = const std::vector<MachineSet>&;
  using FairSets = const std::vector<FairSet>&;
  using Accounts = const std::vector<Account>&;
  using Batches = const std::vector<Batch>&;
  using Jobs = const std::vector<Job>&;

  // In `raw` all objects of the same type must have unique, known ids (id != Id::kNone)
  // and their relations must be valid (other_id references an existing object).
  // Also change_costs_ must hold exactly one element for each possible Change.
  // Otherwise std::invalid_argument will be thrown.
  //
  // If `safe` is set to false objects are allowed to have id == Id::kNone (such objects cannot
  // be retrieved with operator[]) or other_id == Id::kNone (the corresponding method returns
  // default-constructed object). The costs missing from change_costs_ default to 0.
  explicit Situation(const RawSituation &raw, bool safe = true);

  // Copying is pointless as Situation is immutable. Copy constructor and copy assignment operator
  // would be just syntax sugar for keeping std::shared_ptr<Situation>.
  // They might be added in the future.
  Situation(const Situation &) = delete;
  Situation& operator=(const Situation &) = delete;

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

  const ChangeCosts& change_costs() const { return change_costs_; }

 private:
  template<class T>
  static T Get(const std::vector<T> &from, Id<T> id);

  void AddMachines(const std::vector<RawMachine> &raw, bool safe);
  void AddMachineSets(const std::vector<RawMachineSet> &raw, bool safe);
  void AddFairSets(const std::vector<RawFairSet> &raw, bool safe);
  void AddAccounts(const std::vector<RawAccount> &raw, bool safe);
  void AddBatches(const std::vector<RawBatch> &raw, bool safe);
  void AddJobs(const std::vector<RawJob> &raw, bool safe);

  Time time_stamp_;

  std::vector<Machine> machines_;
  std::vector<MachineSet> machine_sets_;
  std::vector<FairSet> fair_sets_;
  std::vector<Account> accounts_;
  std::vector<Batch> batches_;
  std::vector<Job> jobs_;

  ChangeCosts change_costs_;
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
  std::vector<Job> jobs;
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
inline MachineSet::Jobs MachineSet::jobs() const { return data_->jobs; }

inline Id<FairSet> FairSet::id() const { return data_->id; }
inline FairSet::Machines FairSet::machines() const { return data_->machines; }

inline Id<Account> Account::id() const { return data_->id; }
inline FloatType Account::alloc() const { return data_->alloc; }
inline Account::Batches Account::batches() const { return data_->batches; }

inline Id<Batch> Batch::id() const { return data_->id; }
inline FloatType Batch::job_reward() const { return data_->job_reward; }
inline FloatType Batch::job_timely_reward() const { return data_->job_timely_reward; }
inline FloatType Batch::reward() const { return data_->reward; }
inline FloatType Batch::timely_reward() const { return data_->timely_reward; }
inline Duration Batch::duration() const { return data_->duration; }
inline Time Batch::due() const { return data_->due_time; }
inline Account Batch::account() const { return data_->account; }
inline Batch::Jobs Batch::jobs() const { return data_->jobs; }

inline Id<Job> Job::id() const { return data_->id; }
inline Time Job::start_time() const { return data_->start_time; }
inline Duration Job::duration() const { return data_->duration; }
inline Context Job::context() const { return data_->context; }
inline Machine Job::machine() const { return data_->machine; }
inline MachineSet Job::machine_set() const { return data_->machine_set; }
inline Batch Job::batch() const { return data_->batch; }

template<class T>
T Situation::Get(const std::vector<T> &from, Id<T> id) {
  if (!id)
    return T();

  // We'd use lower_bound but it would require creating special dummy object for holding id.
  auto lo = from.begin(), hi = from.end();
  while (hi - lo > 1) {
    auto mid = lo + (hi - lo) / 2;
    if (id < mid->id())
      hi = mid;
    else
      lo = mid;
  }

  if (lo == hi || lo->id() != id)
    return T();
  else
    return *lo;
}

}  // namespace lss

#endif  // LSS_BASE_SITUATION_H_
#ifndef SITUATION_H_
#define SITUATION_H_

#include "experimental/base.h"
#include "experimental/range.h"

#include <functional>
#include <utility>

class Situation {
 public:
  template<class T>
  class Iterator : public std::iterator<std::forward_iterator_tag, T> {};

  Situation();
  Situation(const Situation &old, const RawInput &input, const RawOutput &output);

  Time GetTimeStamp() const;
  const ContextChangeCosts &GetContextChangeCosts() const;

  Range<Iterator<Account>> GetAccounts() const;
  Range<Iterator<Batch>> GetBatches() const;
  Range<Iterator<Job>> GetJobs();
  Range<Iterator<Machine>> GetMachines() const;
  Range<Iterator<MachineSet>> GetMachineSets() const;
  Range<Iterator<FairSet>> GetFairSets() const;
  
 private:
  friend class Account;
  friend class Batch;
  friend class Job;
  friend class Machine;
  friend class MachineSet;
  friend class FairSet;
};

class Context {
 public:
  IdType Get(size_t idx) const;

  static Context Unknown();
};

class ContextChange {
 public:
  ContextChange(Context before, Context after);

  bool IsChanged(size_t idx) const;
};

class ContextChangeCosts {
 public:
  CostType Get(ContextChange context_change) const;
  CostType Get(Context before, Context after) const;
};

class Account {
 public:
  IdType GetId(const Situation &s) const;
  Range<Situation::Iterator<Batch>> GetBatches(const Situation &s) const;

  FloatType GetAlloc(const Situation &s) const;
};

class Batch {
 public:
  IdType GetId(const Situation &s) const;
  Account GetAccount(const Situation &s) const;
  Range<Situation::Iterator<Job>> GetJobs(const Situation &s) const;

  FloatType GetReward(const Situation &s) const;
  FloatType GetTimelyReward(const Situation &s) const;
  FloatType GetJobReward(const Situation &s) const;
  FloatType GetJobTimelReward(const Situation &s) const;
  FloatType GetExpectedDuration(const Situation &s) const;
  FloatType GetDueTime(const Situation &s) const;
};

class Job {
 public:
  IdType GetId(const Situation &s) const;
  Batch GetBatch(const Situation &s) const;
  MachineSet GetMachineSet(const Situation &s) const;
  Machine GetMachine(const Situation &s) const;

  FloatType GetExpectedDuration(const Situation &s) const;
  Context GetContext(const Situation &s) const;
  JobState GetJobState(const Situation &s) const;

  CostType GetContextChangeCost(const Situation &s, Machine m) const;
};

class Machine {
 public:
  IdType GetId(const Situation &s) const;
  Range<Situation::Iterator<MachineSet>> GetMachineSets(const Situation &s) const;
  FairSet GetFairSet(const Situation &s) const;

  MachineState GetState(const Situation &s) const;
  Context GetContext(const Situation &s) const;

  CostType GetContextChangeCost(const Situation &s, Context c) const;
};

class MachineSet {
 public:
  IdType GetId(const Situation &s) const;
  Range<Situation::Iterator<Machine>> GetMachines(const Situation &s) const;
  Range<Situation::Iterator<Job>> GetJobs(const Situation &s) const;
};

class FairSet {
 public:
  IdType GetId(const Situation &s) const;
  Range<Situation::Iterator<Machine>> GetMachines(const Situation &s) const;
};

inline bool operator<(const Account& lhs, const Account& rhs);
inline bool operator<(const Batch& lhs, const Batch& rhs);
inline bool operator<(const Job& lhs, const Job& rhs);
inline bool operator<(const Machine& lhs, const Machine& rhs);
inline bool operator<(const MachineSet& lhs, const MachineSet& rhs);
inline bool operator<(const FairSet& lhs, const FairSet& rhs);

inline bool operator==(const Account& lhs, const Account& rhs);
inline bool operator==(const Batch& lhs, const Batch& rhs);
inline bool operator==(const Job& lhs, const Job& rhs);
inline bool operator==(const Machine& lhs, const Machine& rhs);
inline bool operator==(const MachineSet& lhs, const MachineSet& rhs);
inline bool operator==(const FairSet& lhs, const FairSet& rhs);

using namespace std::rel_ops;

namespace impl {

template<class T>
struct IdHash {
  size_t operator()(const T& t) const { return std::hash<IdType>()(t.GetId()); }
};

}  // namespace impl

namespace std {

template<> struct hash<Account> : public impl::IdHash<Account> {};
template<> struct hash<Batch> : public impl::IdHash<Batch> {};
template<> struct hash<Job> : public impl::IdHash<Job> {};
template<> struct hash<Machine> : public impl::IdHash<Machine> {};
template<> struct hash<MachineSet> : public impl::IdHash<MachineSet> {};
template<> struct hash<FairSet> : public impl::IdHash<FairSet> {};

}  // namespace std

#endif  // SITUATION_H_
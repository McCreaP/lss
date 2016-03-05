#ifndef OO_TYPES_H_
#define OO_TYPES_H_

#include "collection.h"

#include <functional>
#include <utility>

using IdType = int;
using FloatType = double;
using ContextType = int;

enum class MachineState;

class Account;
class Batch;
class Job;
class Machine;
class MachineSet;
class FairMachineSet;

class Situation {
 public:
  Collection<Account> accounts();
  Collection<Batch> batches();
  Collection<Job> jobs();
  Collection<Machine> machines();
  Collection<MachineSet> machine_sets();
  Collection<FairMachineSet> fair_machine_sets();

  Account AccountById(IdType id);
  Batch BatchById(IdType id);
  Job JobById(IdType id);
  Machine MachineById(IdType id);
  MachineSet MachineSetById(IdType id);
  FairMachineSet FairMachineSetById(IdType id);
};

class Account {
 public:
  IdType id() const;
  Collection<Batch> batches() const;

  FloatType alloc() const;
};

class Batch {
 public:
  IdType id() const;
  Account account() const;
  Collection<Job> jobs() const;

  FloatType reward() const;
  FloatType timely_reward() const;
  FloatType job_reward() const;
  FloatType job_timely_reward() const;
  FloatType expected_duration() const;
  FloatType due_time() const;
};

class Job {
 public:
  IdType id() const;
  Batch batch() const;
  MachineSet machine_set() const;

  FloatType expected_duration() const;
  ContextType context1() const;
  ContextType context2() const;
  ContextType context3() const;
};

class Machine {
 public:
  IdType id() const;
  Collection<MachineSet> machine_sets() const;
  FairMachineSet fair_machine_set() const;

  MachineState state() const;
};

class MachineSet {
 public:
  IdType id() const;
  Collection<Machine> machines() const;
  Collection<Job> jobs() const;
};

class FairMachineSet {
 public:
  IdType id() const;
  Collection<Machine> machines() const;
};

inline bool operator<(const Account& lhs, const Account& rhs);
inline bool operator<(const Batch& lhs, const Batch& rhs);
inline bool operator<(const Job& lhs, const Job& rhs);
inline bool operator<(const Machine& lhs, const Machine& rhs);
inline bool operator<(const MachineSet& lhs, const MachineSet& rhs);
inline bool operator<(const FairMachineSet& lhs, const FairMachineSet& rhs);

inline bool operator==(const Account& lhs, const Account& rhs);
inline bool operator==(const Batch& lhs, const Batch& rhs);
inline bool operator==(const Job& lhs, const Job& rhs);
inline bool operator==(const Machine& lhs, const Machine& rhs);
inline bool operator==(const MachineSet& lhs, const MachineSet& rhs);
inline bool operator==(const FairMachineSet& lhs, const FairMachineSet& rhs);

using namespace std::rel_ops;

namespace impl {

template<class T>
struct IdHash {
  size_t operator()(const T& t) const { return std::hash<IdType>()(t.id()); }
};

}  // namespace impl

namespace std {

template<> struct hash<Account> : public impl::IdHash<Account> {};
template<> struct hash<Batch> : public impl::IdHash<Batch> {};
template<> struct hash<Job> : public impl::IdHash<Job> {};
template<> struct hash<Machine> : public impl::IdHash<Machine> {};
template<> struct hash<MachineSet> : public impl::IdHash<MachineSet> {};
template<> struct hash<FairMachineSet> : public impl::IdHash<FairMachineSet> {};

}  // namespace std

#endif  // OO_TYPES_H_
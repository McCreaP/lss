#include "base/situation.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>

namespace lss {
namespace {

struct Throw {};

class Exception {
 public:
  template<class T>
  Exception& operator<<(T val) { what_ << val; return *this; }

  void operator<<(Throw) { throw std::invalid_argument(what_.str()); }

 private:
  std::ostringstream what_;
};

template<class T>
bool IdCmp(T lhs, T rhs)  { return lhs.id() < rhs.id(); }

template<class T>
inline void Sort(std::vector<T> *vec) {
  std::sort(vec->begin(), vec->end(), &IdCmp<T>);
}

template<class T>
void SortAndVerify(std::vector<T> *vec, bool safe) {
  Sort(vec);

  // We rely on the fact that Id::kNone has the smallest possible value.
  if (safe && !vec->empty() && !vec->front().id())
    Exception() << "Object with id == kNone" << Throw();

  for (size_t i = 1; i < vec->size(); ++i)
    if ((*vec)[i-1].id() == (*vec)[i].id() && (*vec)[i].id())
      Exception() << "Multiple objects with the same id (" << static_cast<int>((*vec)[i].id())
          << ")" << Throw();
}

}  // namespace

ChangeCosts::ChangeCosts(const std::vector<RawChangeCost> &raw, bool safe) {
  if (safe && raw.size() != Change::kNum)
    Exception() << "Missing costs for changes" << Throw();

  bool found[Change::kNum] = {};
  for (auto &change : raw) {
    size_t idx = static_cast<size_t>(change.change_);
    if (found[idx])
      Exception() << "Multiple costs for single change" << Throw();

    found[idx] = true;
    cost_[idx] = change.cost_;
  }
}

Situation::Situation(const RawSituation &raw, bool safe)
    : data_(std::make_shared<Data>(raw.change_costs_, safe)) {
  data_->time_stamp_ = raw.time_stamp_;

  // The order of adding is important - we rely on the fact, that the (one-way) relations
  // form an acyclic graph.
  try {
    AddMachines(raw.machines_, safe);
    AddMachineSets(raw.machine_sets_, safe);
    AddFairSets(raw.fair_sets_, safe);
    AddAccounts(raw.accounts_, safe);
    AddBatches(raw.batches_, safe);
    AddJobs(raw.jobs_, safe);
  } catch (...) {
    FreeMem();
    throw;
  }

  // The general vectors containing all the objects are already sorted by calls to Add*,
  // so we just sort the vectors inside the objects.
  for (auto m : data_->machines_)
    Sort(&m.data_->machine_sets);
  for (auto s : data_->machine_sets_) {
    Sort(&s.data_->machines);
    Sort(&s.data_->jobs);
  }
  for (auto f : data_->fair_sets_)
    Sort(&f.data_->machines);
  for (auto a : data_->accounts_)
    Sort(&a.data_->batches);
  for (auto b : data_->batches_)
    Sort(&b.data_->jobs);
  // Only single relations in Job, so no loop for it.
}

void Situation::FreeMem() {
  for (auto m : data_->machines_) delete m.data_;
  for (auto s : data_->machine_sets_) delete s.data_;
  for (auto f : data_->fair_sets_) delete f.data_;
  for (auto a : data_->accounts_) delete a.data_;
  for (auto b : data_->batches_) delete b.data_;
  for (auto j : data_->jobs_) delete j.data_;
}

void Situation::AddMachines(const std::vector<RawMachine> &raw, bool safe) {
  data_->machines_.reserve(raw.size());
  for (auto &rm : raw) {
    Machine m(new Machine::Data);
    data_->machines_.push_back(m);

    m.data_->id = Id<Machine>(rm.id_);
    m.data_->state = rm.state_;
    m.data_->context = rm.context_;
  }
  SortAndVerify(&data_->machines_, safe);
}

void Situation::AddMachineSets(const std::vector<RawMachineSet> &raw, bool safe) {
  data_->machine_sets_.reserve(raw.size());
  for (auto &rs : raw) {
    MachineSet s(new MachineSet::Data);
    data_->machine_sets_.push_back(s);

    s.data_->id = Id<MachineSet>(rs.id_);
    s.data_->machines.reserve(rs.machines_.size());
    for (IdType m_id : rs.machines_) {
      if (Machine m = (*this)[Id<Machine>(m_id)]) {
        s.data_->machines.push_back(m);
        m.data_->machine_sets.push_back(s);
      } else if (safe || m_id != kIdNone) {
        Exception() << "Invalid relation: MachineSet " << rs.id_
            << " refers to nonexistent Machine " << m_id << Throw();
      }
    }
  }
  SortAndVerify(&data_->machine_sets_, safe);
}

void Situation::AddFairSets(const std::vector<RawFairSet> &raw, bool safe) {
  data_->fair_sets_.reserve(raw.size());
  for (auto &rf : raw) {
    FairSet f(new FairSet::Data);
    data_->fair_sets_.push_back(f);

    f.data_->id = Id<FairSet>(rf.id_);
    f.data_->machines.reserve(rf.machines_.size());
    for (IdType m_id : rf.machines_) {
      if (Machine m = (*this)[Id<Machine>(m_id)]) {
        if (m.fair_set())
          Exception() << "FairSets " << rf.id_ << " and " << static_cast<IdType>(m.fair_set().id())
              << " overlap on Machine " << m_id << Throw();
        f.data_->machines.push_back(m);
        m.data_->fair_set = f;
      } else if (safe || m_id != kIdNone) {
        Exception() << "Invalid relation: FairSet " << static_cast<IdType>(f.id())
            << " refers to nonexistent Machine " << m_id << Throw();
      }
    }
  }
  SortAndVerify(&data_->fair_sets_, safe);
}

void Situation::AddAccounts(const std::vector<RawAccount> &raw, bool safe) {
  data_->accounts_.reserve(raw.size());
  for (auto &ra : raw) {
    Account a(new Account::Data);
    data_->accounts_.push_back(a);

    a.data_->id = Id<Account>(ra.id_);
    a.data_->alloc = ra.alloc_;
  }
  SortAndVerify(&data_->accounts_, safe);
}

void Situation::AddBatches(const std::vector<RawBatch> &raw, bool safe) {
  data_->batches_.reserve(raw.size());
  for (auto &rb : raw) {
    Batch b(new Batch::Data);
    data_->batches_.push_back(b);

    b.data_->id = Id<Batch>(rb.id_);
    b.data_->reward = rb.reward_;
    b.data_->timely_reward = rb.timely_reward_;
    b.data_->job_reward = rb.job_reward_;
    b.data_->job_timely_reward = rb.job_timely_reward_;
    b.data_->duration = rb.duration_;
    b.data_->due_time = rb.due_;
    if (Account a = (*this)[Id<Account>(rb.account_)]) {
      b.data_->account = a;
      a.data_->batches.push_back(b);
    } else if (safe || rb.account_ != kIdNone) {
      Exception() << "Invalid relation: Batch " << rb.id_ << " refers to nonexistent Account "
          << rb.account_ << Throw();
    }
  }
  SortAndVerify(&data_->batches_, safe);
}

void Situation::AddJobs(const std::vector<RawJob> &raw, bool safe) {
  data_->jobs_.reserve(raw.size());
  for (auto &rj : raw) {
    Job j(new Job::Data);
    data_->jobs_.push_back(j);

    j.data_->id = Id<Job>(rj.id_);
    j.data_->duration = rj.duration_;
    j.data_->context = rj.context_;
    j.data_->start_time = rj.start_time_;

    if (Machine m = (*this)[Id<Machine>(rj.machine_)]) {
      if (m.job())
        Exception() << "Machine " << rj.machine_ << " has multiple jobs (" << rj.id_
            << " and " << static_cast<IdType>(m.job().id()) << ")" << Throw();
      j.data_->machine = m;
      m.data_->job = j;
    } else if (rj.machine_ != kIdNone) {
      Exception() << "Invalid relation: Job " << static_cast<IdType>(j.id())
          << " refers to nonexistent Machine " << rj.machine_ << Throw();
    }

    if (MachineSet s = (*this)[Id<MachineSet>(rj.machine_set_)]) {
      j.data_->machine_set = s;
      s.data_->jobs.push_back(j);
    } else if (safe || rj.machine_set_ != kIdNone) {
      Exception() << "Invalid relation: Job " << static_cast<IdType>(j.id())
          << " refers to nonexistent MachineSet " << rj.machine_set_ << Throw();
    }

    if (Batch b = (*this)[Id<Batch>(rj.batch_)]) {
      j.data_->batch = b;
      b.data_->jobs.push_back(j);
    } else if (safe || rj.batch_ != kIdNone) {
      Exception() << "Invalid relation: Job " << static_cast<IdType>(j.id())
          << " refers to nonexistent Batch " << rj.batch_ << Throw();
    }
  }
  SortAndVerify(&data_->jobs_, safe);
}

}  // namespace lss

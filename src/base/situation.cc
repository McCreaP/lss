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
void SortAndVerify(std::vector<T> *vec, Situation::BuildMode mode) {
  Sort(vec);

  // We rely on the fact that Id::kNone has the smallest possible value.
  if (mode != Situation::BuildMode::kIgnoreMissing && !vec->empty() && !vec->front().id())
    Exception() << "Object with id == kNone" << Throw();

  for (size_t i = 1; i < vec->size(); ++i)
    if ((*vec)[i-1].id() == (*vec)[i].id() && (*vec)[i].id())
      Exception() << "Multiple objects with the same id (" << static_cast<IdType>((*vec)[i].id())
          << ")" << Throw();
}

}  // namespace

ChangeCosts::ChangeCosts(const std::vector<RawChangeCost> &raw, bool allow_missing) {
  if (!allow_missing && raw.size() != Change::kNum)
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

Situation::Situation(const RawSituation &raw, BuildMode mode)
    : data_(std::make_shared<Data>(raw.change_costs_, mode)) {
  data_->time_stamp_ = raw.time_stamp_;

  // The order of adding is important - we rely on the fact, that the (one-way) relations
  // form an acyclic graph.
  try {
    AddMachines(raw.machines_, mode);
    AddMachineSets(raw.machine_sets_, mode);
    AddFairSets(raw.fair_sets_, mode);
    AddAccounts(raw.accounts_, mode);
    AddBatches(raw.batches_, mode);
    AddJobs(raw.jobs_, mode);
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

Situation::Situation(const RawSituation &raw, bool safe)
    : Situation(raw, safe ? BuildMode::kSafe : BuildMode::kIgnoreMissing) {}

void Situation::FreeMem() {
  for (auto m : data_->machines_) delete m.data_;
  for (auto s : data_->machine_sets_) delete s.data_;
  for (auto f : data_->fair_sets_) delete f.data_;
  for (auto a : data_->accounts_) delete a.data_;
  for (auto b : data_->batches_) delete b.data_;
  for (auto j : data_->jobs_) delete j.data_;
}

void Situation::AddMachines(const std::vector<RawMachine> &raw, BuildMode mode) {
  data_->machines_.reserve(raw.size());
  for (auto &rm : raw) {
    Machine m(new Machine::Data);
    data_->machines_.push_back(m);

    m.data_->id = Id<Machine>(rm.id_);
    m.data_->state = rm.state_;
    m.data_->context = rm.context_;
  }
  SortAndVerify(&data_->machines_, mode);
}

void Situation::AddMachineSets(const std::vector<RawMachineSet> &raw, BuildMode mode) {
  data_->machine_sets_.reserve(raw.size());
  for (auto &raw_set : raw) {
    MachineSet set(new MachineSet::Data);
    data_->machine_sets_.push_back(set);

    set.data_->id = Id<MachineSet>(raw_set.id_);
    set.data_->machines.reserve(raw_set.machines_.size());
    for (IdType machine_id : raw_set.machines_) {
      if (Machine machine = (*this)[Id<Machine>(machine_id)]) {
        set.data_->machines.push_back(machine);
        machine.data_->machine_sets.push_back(set);
      } else if (mode == BuildMode::kSafe
          || (mode == BuildMode::kIgnoreMissing && machine_id != kIdNone)) {
        Exception() << "Invalid relation: MachineSet " << raw_set.id_
            << " refers to nonexistent Machine " << machine_id << Throw();
      }
    }
  }
  SortAndVerify(&data_->machine_sets_, mode);
}

void Situation::AddFairSets(const std::vector<RawFairSet> &raw, BuildMode mode) {
  data_->fair_sets_.reserve(raw.size());
  for (auto &raw_set : raw) {
    FairSet set(new FairSet::Data);
    data_->fair_sets_.push_back(set);

    set.data_->id = Id<FairSet>(raw_set.id_);
    set.data_->machines.reserve(raw_set.machines_.size());
    for (IdType machine_id : raw_set.machines_) {
      Machine machine = (*this)[Id<Machine>(machine_id)];

      if (!machine) {
        bool ok = false;
        ok |= mode == BuildMode::kDropInvalid;
        ok |= mode == BuildMode::kIgnoreMissing && machine_id == kIdNone;
        if (!ok)
          Exception() << "Invalid relation: FairSet " << static_cast<IdType>(set.id())
              << " refers to nonexistent Machine " << machine_id << Throw();
      }

      if (machine && machine.fair_set()) {
        if (mode != BuildMode::kDropInvalid)
          Exception() << "FairSets " << raw_set.id_ << " and "
              << static_cast<IdType>(machine.fair_set().id()) << " overlap on Machine "
              << machine_id << Throw();
        machine = Machine();
      }

      if (machine) {
        set.data_->machines.push_back(machine);
        machine.data_->fair_set = set;
      }
    }
  }
  SortAndVerify(&data_->fair_sets_, mode);
}

void Situation::AddAccounts(const std::vector<RawAccount> &raw, BuildMode mode) {
  data_->accounts_.reserve(raw.size());
  for (auto &ra : raw) {
    Account a(new Account::Data);
    data_->accounts_.push_back(a);

    a.data_->id = Id<Account>(ra.id_);
    a.data_->alloc = ra.alloc_;
  }
  SortAndVerify(&data_->accounts_, mode);
}

void Situation::AddBatches(const std::vector<RawBatch> &raw, BuildMode mode) {
  data_->batches_.reserve(raw.size());
  for (auto &raw_batch : raw) {
    if (mode == BuildMode::kDropInvalid && !(*this)[Id<Account>(raw_batch.account_)])
      continue;

    Batch batch(new Batch::Data);
    data_->batches_.push_back(batch);

    batch.data_->id = Id<Batch>(raw_batch.id_);
    batch.data_->reward = raw_batch.reward_;
    batch.data_->timely_reward = raw_batch.timely_reward_;
    batch.data_->job_reward = raw_batch.job_reward_;
    batch.data_->job_timely_reward = raw_batch.job_timely_reward_;
    batch.data_->duration = raw_batch.duration_;
    batch.data_->due_time = raw_batch.due_;
    if (Account account = (*this)[Id<Account>(raw_batch.account_)]) {
      batch.data_->account = account;
      account.data_->batches.push_back(batch);
    } else if (mode == BuildMode::kSafe
        || (mode == BuildMode::kIgnoreMissing && raw_batch.account_ != kIdNone)) {
      Exception() << "Invalid relation: Batch " << raw_batch.id_
          << " refers to nonexistent Account " << raw_batch.account_ << Throw();
    }
  }
  SortAndVerify(&data_->batches_, mode);
}

void Situation::AddJobs(const std::vector<RawJob> &raw, BuildMode mode) {
  data_->jobs_.reserve(raw.size());
  for (auto &raw_job : raw) {
    if (mode == BuildMode::kDropInvalid
        && !((*this)[Id<MachineSet>(raw_job.machine_set_)] && (*this)[Id<Batch>(raw_job.batch_)]))
      continue;

    Job job(new Job::Data);
    data_->jobs_.push_back(job);

    job.data_->id = Id<Job>(raw_job.id_);
    job.data_->duration = raw_job.duration_;
    job.data_->context = raw_job.context_;
    job.data_->start_time = raw_job.start_time_;

    if (Machine machine = (*this)[Id<Machine>(raw_job.machine_)]) {
      if (machine.job())
        Exception() << "Machine " << raw_job.machine_ << " has multiple jobs (" << raw_job.id_
            << " and " << static_cast<IdType>(machine.job().id()) << ")" << Throw();
      job.data_->machine = machine;
      machine.data_->job = job;
    } else if (mode != BuildMode::kDropInvalid && raw_job.machine_ != kIdNone) {
      Exception() << "Invalid relation: Job " << static_cast<IdType>(job.id())
          << " refers to nonexistent Machine " << raw_job.machine_ << Throw();
    }

    if (MachineSet set = (*this)[Id<MachineSet>(raw_job.machine_set_)]) {
      job.data_->machine_set = set;
      set.data_->jobs.push_back(job);
    } else if (mode == BuildMode::kSafe
        || (mode == BuildMode::kIgnoreMissing && raw_job.machine_set_ != kIdNone)) {
      Exception() << "Invalid relation: Job " << raw_job.id_
          << " refers to nonexistent MachineSet " << raw_job.machine_set_ << Throw();
    }

    if (Batch batch = (*this)[Id<Batch>(raw_job.batch_)]) {
      job.data_->batch = batch;
      batch.data_->jobs.push_back(job);
    } else if (mode == BuildMode::kSafe
        || (mode == BuildMode::kIgnoreMissing && raw_job.batch_ != kIdNone)) {
      Exception() << "Invalid relation: Job " << raw_job.id_
          << " refers to nonexistent Batch " << raw_job.batch_ << Throw();
    }
  }
  SortAndVerify(&data_->jobs_, mode);
}

}  // namespace lss

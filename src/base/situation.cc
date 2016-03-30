#include "base/situation.h"

#include <algorithm>

namespace lss {
namespace {

template<class T>
bool IdCmp(T lhs, T rhs)  { return lhs.id() < rhs.id(); }

}  // namespace

Situation::Situation(const RawSituation &raw) {
  time_stamp_ = raw.time_stamp;

  // The order of adding is important - we rely on the fact, that the (one-way) relations
  // form an acyclic graph.
  AddMachines(raw.machines);
  AddMachineSets(raw.machine_sets);
  AddFairSets(raw.fair_sets);
  AddAccounts(raw.accounts);
  AddBatches(raw.batches);
  AddJobs(raw.jobs);
}

void Situation::AddMachines(const std::vector<RawMachine> &raw) {
  machines_.reserve(raw.size());
  for (auto &rm : raw) {
    Machine m(new Machine::Data);
    machines_.push_back(m);

    m.data_->id = Id<Machine>(rm.id);
    m.data_->state = rm.state;
    m.data_->context = rm.context;
  }
  std::sort(machines_.begin(), machines_.end(), &IdCmp<Machine>);
}

void Situation::AddMachineSets(const std::vector<RawMachineSet> &raw) {
  machine_sets_.reserve(raw.size());
  for (auto &rs : raw) {
    MachineSet s(new MachineSet::Data);
    machine_sets_.push_back(s);

    s.data_->id = Id<MachineSet>(rs.id);
    s.data_->machines.reserve(rs.machines.size());
    for (IdType m_id : rs.machines) {
      if (Machine m = (*this)[Id<Machine>(m_id)]) {
        s.data_->machines.push_back(m);
        m.data_->machine_sets.push_back(s);
      }
    }
  }
  std::sort(machine_sets_.begin(), machine_sets_.end(), &IdCmp<MachineSet>);
}

void Situation::AddFairSets(const std::vector<RawMachineSet> &raw) {
  fair_sets_.reserve(raw.size());
  for (auto &rf : raw) {
    FairSet f(new FairSet::Data);
    fair_sets_.push_back(f);

    f.data_->id = Id<FairSet>(rf.id);
    f.data_->machines.reserve(rf.machines.size());
    for (IdType m_id : rf.machines) {
      if (Machine m = (*this)[Id<Machine>(m_id)]) {
        f.data_->machines.push_back(m);
        m.data_->fair_set = f;
      }
    }
  }
  std::sort(fair_sets_.begin(), fair_sets_.end(), &IdCmp<FairSet>);
}

void Situation::AddAccounts(const std::vector<RawAccount> &raw) {
  accounts_.reserve(raw.size());
  for (auto &ra : raw) {
    Account a(new Account::Data);
    accounts_.push_back(a);

    a.data_->id = Id<Account>(ra.id);
    a.data_->alloc = ra.alloc;
  }
  std::sort(accounts_.begin(), accounts_.end(), &IdCmp<Account>);
}

void Situation::AddBatches(const std::vector<RawBatch> &raw) {
  batches_.reserve(raw.size());
  for (auto &rb : raw) {
    Batch b(new Batch::Data);
    batches_.push_back(b);

    b.data_->id = Id<Batch>(rb.id);
    b.data_->reward = rb.reward;
    b.data_->timely_reward = rb.timely_reward;
    b.data_->job_reward = rb.job_reward;
    b.data_->job_timely_reward = rb.job_timely_reward;
    b.data_->duration = rb.expected_time;
    b.data_->due_time = rb.due;
    if (Account a = (*this)[Id<Account>(rb.account_id)]) {
      b.data_->account = a;
      a.data_->batches.push_back(b);
    }
  }
  std::sort(batches_.begin(), batches_.end(), &IdCmp<Batch>);
}

void Situation::AddJobs(const std::vector<RawJob> &raw) {
  jobs_.reserve(raw.size());
  for (auto &rj : raw) {
    Job j(new Job::Data);
    jobs_.push_back(j);

    j.data_->id = Id<Job>(rj.id);
    j.data_->duration = rj.duration;
    j.data_->context = rj.context;
    j.data_->start_time = rj.start_time;
    if (Machine m = (*this)[Id<Machine>(rj.machine_id)]) {
      j.data_->machine = m;
      m.data_->job = j;
    }
    if (MachineSet s = (*this)[Id<MachineSet>(rj.machineset_id)]) {
      j.data_->machine_set = s;
      s.data_->jobs.push_back(j);
    }
    if (Batch b = (*this)[Id<Batch>(rj.batch_id)]) {
      j.data_->batch = b;
      b.data_->jobs.push_back(j);
    }
  }
  std::sort(jobs_.begin(), jobs_.end(), &IdCmp<Job>);
}

Situation::~Situation() {
  for (auto m : machines_) delete m.data_;
  for (auto s : machine_sets_) delete s.data_;
  for (auto f : fair_sets_) delete f.data_;
  for (auto a : accounts_) delete a.data_;
  for (auto b : batches_) delete b.data_;
  for (auto j : jobs_) delete j.data_;
}

template<class T>
T Situation::Get(const std::vector<T> &from, Id<T> id) {
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

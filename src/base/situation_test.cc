#include "base/situation.h"

#include <algorithm>

#include "gtest/gtest.h"

namespace lss {
namespace {

template<class Type>
bool IdEqual(const std::vector<IdType> &v1, const std::vector<Type> &v2) {
  if (v1.size() != v2.size())
    return false;
  auto it1 = v1.begin();
  auto it2 = v2.begin();
  while (it1 != v1.end()) {
    if (Id<Type>(*it1) != it2->id())
      return false;
    ++it1; ++it2;
  }
  return true;
}

template<class RawType, class Type>
bool IdEqual(const std::vector<RawType> &v1, const std::vector<Type> &v2) {
  if (v1.size() != v2.size())
    return false;
  auto it1 = v1.begin();
  auto it2 = v2.begin();
  while (it1 != v1.end()) {
    if (Id<Type>(it1->id_) != it2->id())
      return false;
    ++it1; ++it2;
  }
  return true;
}

template<class Type>
bool Contains(Type what, const std::vector<Type> &where) {
  for (auto val : where)
    if (val.id() == what.id()) return true;
  return false;
}

template<class Type>
bool IsSorted(const std::vector<Type> &vec) {
  for (size_t i = 1; i < vec.size(); ++i)
    if (vec[i-1].id() >= vec[i].id()) return false;
  return true;
}

const RawSituation sample = RawSituation()
    .time_stamp(1)
    .add(RawMachine().id(2).state(MachineState::kIdle).context(Context(3, 4, 5)))
    .add(RawMachine().id(6).state(MachineState::kWorking).context(Context(7, 8, 9)))
    .add(RawMachine().id(10).state(MachineState::kDead).context(Context(11, 12, 13)))
    .add(RawMachineSet().id(14).add(2).add(6))
    .add(RawMachineSet().id(15).add(6).add(10))
    .add(RawFairSet().id(16).add(2).add(6))
    .add(RawFairSet().id(17).add(10))
    .add(RawAccount().id(18).alloc(19))
    .add(RawAccount().id(20).alloc(21))
    .add(RawBatch()
        .id(22).job_reward(23).job_timely_reward(24).reward(25).timely_reward(26)
        .duration(27).due(28).account(18))
    .add(RawBatch()
        .id(29).job_reward(30).job_timely_reward(31).reward(32).timely_reward(33)
        .duration(34).due(35).account(18))
    .add(RawJob()
        .id(36).duration(37).context(Context(38, 39, 40)).start_time(41)
        .machine(2).machine_set(14).batch(22))
    .add(RawJob()
        .id(42).duration(43).context(Context(44, 45, 46)).start_time(47)
        .machine(10).machine_set(15).batch(22))
    .add(RawChangeCost().change(Change(0, 0, 0)).cost(48))
    .add(RawChangeCost().change(Change(0, 0, 1)).cost(49))
    .add(RawChangeCost().change(Change(0, 1, 0)).cost(50))
    .add(RawChangeCost().change(Change(0, 1, 1)).cost(51))
    .add(RawChangeCost().change(Change(1, 0, 0)).cost(52))
    .add(RawChangeCost().change(Change(1, 0, 1)).cost(53))
    .add(RawChangeCost().change(Change(1, 1, 0)).cost(54))
    .add(RawChangeCost().change(Change(1, 1, 1)).cost(55));

// Verify that no object is missing from Situation.
TEST(SituationTest, NothingMissing) {
  Situation s{sample};
  EXPECT_TRUE(IdEqual(sample.machines_, s.machines()));
  EXPECT_TRUE(IdEqual(sample.machine_sets_, s.machine_sets()));
  EXPECT_TRUE(IdEqual(sample.fair_sets_, s.fair_sets()));
  EXPECT_TRUE(IdEqual(sample.accounts_, s.accounts()));
  EXPECT_TRUE(IdEqual(sample.batches_, s.batches()));
  EXPECT_TRUE(IdEqual(sample.jobs_, s.jobs()));
}

// Verify that collections returned by Situation are correctly sorted.
TEST(SituationTest, SituationVectorsSorted) {
  // Break order to ensure the elements are actually rearranged.
  RawSituation raw = sample;
  std::swap(raw.machines_[0], raw.machines_[2]);
  std::swap(raw.machine_sets_[0], raw.machine_sets_[1]);
  std::swap(raw.fair_sets_[0], raw.fair_sets_[1]);
  std::swap(raw.accounts_[0], raw.accounts_[1]);
  std::swap(raw.batches_[0], raw.batches_[1]);
  std::swap(raw.jobs_[0], raw.jobs_[1]);

  Situation s(raw);
  EXPECT_TRUE(IsSorted(s.machines()));
  EXPECT_TRUE(IsSorted(s.machine_sets()));
  EXPECT_TRUE(IsSorted(s.fair_sets()));
  EXPECT_TRUE(IsSorted(s.accounts()));
  EXPECT_TRUE(IsSorted(s.batches()));
  EXPECT_TRUE(IsSorted(s.jobs()));
}

// Verify that collections returned by objects are correctly sorted.
TEST(SituationTest, ObjectVectorsSorted) {
  Situation situation{sample};
  for (auto m : situation.machines())
    EXPECT_TRUE(IsSorted(m.machine_sets()));
  for (auto s : situation.machine_sets()) {
    EXPECT_TRUE(IsSorted(s.machines()));
    EXPECT_TRUE(IsSorted(s.jobs()));
  }
  for (auto f : situation.fair_sets())
    EXPECT_TRUE(IsSorted(f.machines()));
  for (auto a : situation.accounts())
    EXPECT_TRUE(IsSorted(a.batches()));
  for (auto b : situation.batches())
    EXPECT_TRUE(IsSorted(b.jobs()));
}

class SituationPairwiseTest : public ::testing::Test {
 protected:
  Situation situation_{sample};

  const RawMachine &rm_ = sample.machines_[1];
  Machine m_ = situation_.machines()[1];
  const RawMachineSet &rs_ = sample.machine_sets_[0];
  MachineSet s_ = situation_.machine_sets()[0];
  const RawFairSet &rf_ = sample.fair_sets_[1];
  FairSet f_ = situation_.fair_sets()[1];
  const RawAccount &ra_ = sample.accounts_[0];
  Account a_ = situation_.accounts()[0];
  const RawBatch &rb_ = sample.batches_[1];
  Batch b_ = situation_.batches()[1];
  const RawJob &rj_ = sample.jobs_[0];
  Job j_ = situation_.jobs()[0];
};

// Verify that all "property" fields are correctly copied to Situation.
TEST_F(SituationPairwiseTest, Properties) {
  EXPECT_EQ(sample.time_stamp_, situation_.time_stamp());

  EXPECT_EQ(Id<Machine>(rm_.id_), m_.id());
  EXPECT_EQ(rm_.state_, m_.state());
  EXPECT_EQ(rm_.context_, m_.context());

  EXPECT_EQ(Id<MachineSet>(rs_.id_), s_.id());

  EXPECT_EQ(Id<FairSet>(rf_.id_), f_.id());

  EXPECT_EQ(Id<Account>(ra_.id_), a_.id());
  EXPECT_EQ(ra_.alloc_, a_.alloc());

  EXPECT_EQ(Id<Batch>(rb_.id_), b_.id());
  EXPECT_EQ(rb_.job_reward_, b_.job_reward());
  EXPECT_EQ(rb_.job_timely_reward_, b_.job_timely_reward());
  EXPECT_EQ(rb_.reward_, b_.reward());
  EXPECT_EQ(rb_.timely_reward_, b_.timely_reward());
  EXPECT_EQ(rb_.duration_, b_.duration());
  EXPECT_EQ(rb_.due_, b_.due());

  EXPECT_EQ(Id<Job>(rj_.id_), j_.id());
  EXPECT_EQ(rj_.duration_, j_.duration());
  EXPECT_EQ(rj_.context_, j_.context());
  EXPECT_EQ(rj_.start_time_, j_.start_time());
}

// Verify that forward relations are correctly represented in situation.
TEST_F(SituationPairwiseTest, FordwardRelations) {
  EXPECT_TRUE(IdEqual(rs_.machines_, s_.machines()));
  EXPECT_TRUE(IdEqual(rf_.machines_, f_.machines()));
  EXPECT_EQ(Id<Account>(rb_.account_), b_.account().id());
  EXPECT_EQ(Id<Machine>(rj_.machine_), j_.machine().id());
  EXPECT_EQ(Id<MachineSet>(rj_.machine_set_), j_.machine_set().id());
  EXPECT_EQ(Id<Batch>(rj_.batch_), j_.batch().id());
}

// Verify that backward relations are correctly represented in situation.
TEST_F(SituationPairwiseTest, BackwardRelations) {
  EXPECT_TRUE(Contains(s_, s_.machines().front().machine_sets()));
  EXPECT_EQ(f_.id(), f_.machines().front().fair_set().id());
  EXPECT_EQ(j_.id(), j_.machine().job().id());
  EXPECT_TRUE(Contains(j_, j_.machine_set().jobs()));
  EXPECT_TRUE(Contains(b_, b_.account().batches()));
  EXPECT_TRUE(Contains(j_, j_.batch().jobs()));
}

// Verify that objects are accessible by their id.
TEST(SituationTest, AccessById) {
  Situation situation(sample);

  for (auto &m : sample.machines_)
    if (!situation[Id<Machine>(m.id_)]) ADD_FAILURE();
  for (auto &s : sample.machine_sets_)
    if (!situation[Id<MachineSet>(s.id_)]) ADD_FAILURE();
  for (auto &f : sample.fair_sets_)
    if (!situation[Id<FairSet>(f.id_)]) ADD_FAILURE();
  for (auto &a : sample.accounts_)
    if (!situation[Id<Account>(a.id_)]) ADD_FAILURE();
  for (auto &b : sample.batches_)
    if (!situation[Id<Batch>(b.id_)]) ADD_FAILURE();
  for (auto &j : sample.jobs_)
    if (!situation[Id<Job>(j.id_)]) ADD_FAILURE();
}

class RawWithCosts : public ::testing::Test {
 protected:
  virtual void SetUp() {
    raw_.change_costs_ = sample.change_costs_;
  }

  RawSituation raw_;
};

class SituationSafeThrowTest : public RawWithCosts {};
class SituationThrowTest : public RawWithCosts, public ::testing::WithParamInterface<bool> {};
INSTANTIATE_TEST_CASE_P(SafeUnsafe, SituationThrowTest, ::testing::Bool());

// Verify that Situation constructor throws when some object has default id and safe == true;
TEST_F(SituationSafeThrowTest, DefaultId) {
  EXPECT_THROW({
    Situation(RawSituation(raw_).add(RawMachine()), true);
  }, std::invalid_argument);
  EXPECT_THROW({
    Situation(RawSituation(raw_).add(RawMachineSet()), true);
  }, std::invalid_argument);
  EXPECT_THROW({
    Situation(RawSituation(raw_).add(RawFairSet()), true);
  }, std::invalid_argument);
  EXPECT_THROW({
    Situation(RawSituation(raw_).add(RawAccount()), true);
  }, std::invalid_argument);
  raw_.add(RawAccount().id(0));
  EXPECT_THROW({
    Situation(RawSituation(raw_).add(RawBatch().account(0)), true);
  }, std::invalid_argument);
  raw_.add(RawMachineSet().id(0)).add(RawBatch().id(0).account(0));
  EXPECT_THROW({
    Situation(RawSituation(raw_).add(RawJob().batch(0).machine_set(0)), true);
  }, std::invalid_argument);
}

// Verify that Situation constructor throws when multiple objects have the same id.
TEST_P(SituationThrowTest, SameId) {
  EXPECT_THROW({
    Situation(RawSituation(raw_).add(RawMachine().id(1)).add(RawMachine().id(1)), GetParam());
  }, std::invalid_argument);
  EXPECT_THROW({
    auto raw = RawSituation(raw_).add(RawMachineSet().id(7)).add(RawMachineSet().id(7));
    Situation(raw, GetParam());
  }, std::invalid_argument);
  EXPECT_THROW({
    Situation(RawSituation(raw_).add(RawFairSet().id(2)).add(RawFairSet().id(2)), GetParam());
  }, std::invalid_argument);
  EXPECT_THROW({
    Situation(RawSituation(raw_).add(RawAccount().id(3)).add(RawAccount().id(3)), GetParam());
  }, std::invalid_argument);
  raw_.add(RawAccount().id(0));
  EXPECT_THROW({
    auto raw = RawSituation(raw_).add(RawBatch().id(4).account(0)).add(RawBatch().id(4).account(0));
    Situation(raw, GetParam());
  }, std::invalid_argument);
  raw_.add(RawMachineSet().id(0)).add(RawBatch().id(0).account(0));
  EXPECT_THROW({
    auto raw = RawSituation(raw_)
        .add(RawJob().id(6).batch(0).machine_set(0))
        .add(RawJob().id(6).batch(0).machine_set(0));
    Situation(raw, GetParam());
  }, std::invalid_argument);
}

// Verify that Situation constructor throws when non-optional relation is missing and safe == true.
TEST_F(SituationSafeThrowTest, MissingRelation) {
  EXPECT_THROW({
    Situation(RawSituation(raw_).add(RawBatch().id(0)));
  }, std::invalid_argument);
  raw_.add(RawAccount().id(0)).add(RawBatch().id(0).account(0)).add(RawMachineSet().id(0));
  EXPECT_THROW({
    Situation(RawSituation(raw_).add(RawJob().id(0).machine_set(0)));
  }, std::invalid_argument);
  EXPECT_THROW({
    Situation(RawSituation(raw_).add(RawJob().id(0).batch(0)));
  }, std::invalid_argument);
}

// Verify that Situation constructor throws when some relation is invalid.
TEST_P(SituationThrowTest, Invalidrelation) {
  EXPECT_THROW({
    Situation(RawSituation(raw_).add(RawMachineSet().id(0).add(1)), GetParam());
  }, std::invalid_argument);
  EXPECT_THROW({
    Situation(RawSituation(raw_).add(RawFairSet().id(0).add(1)), GetParam());
  }, std::invalid_argument);
  EXPECT_THROW({
    Situation(RawSituation(raw_).add(RawBatch().id(0).account(1)), GetParam());
  }, std::invalid_argument);
  raw_.add(RawAccount().id(0)).add(RawBatch().id(0).account(0)).add(RawMachine().id(0));
  EXPECT_THROW({
    Situation(RawSituation(raw_).add(RawJob().id(0).batch(0).machine_set(1)), GetParam());
  }, std::invalid_argument);
  EXPECT_THROW({
    Situation(RawSituation(raw_).add(RawJob().id(0).batch(1).machine_set(0)), GetParam());
  }, std::invalid_argument);

}

// Verify that Situation constructor throws when multiple costs are given for one change.
TEST_P(SituationThrowTest, MultipleCosts) {
  raw_.change_costs_.push_back(raw_.change_costs_.back());
  EXPECT_THROW({
    Situation(raw_, GetParam());
  }, std::invalid_argument);
}

// Verify that Situation constructor throws when costs are missing and safe == true.
TEST_F(SituationSafeThrowTest, MissingCost) {
  raw_.change_costs_.pop_back();
  EXPECT_THROW({
    Situation(raw_, true);
  }, std::invalid_argument);
}

// Verify that Situation constructor throws when fair sets are not disjoint.
TEST_P(SituationThrowTest, OverlappingFairSets) {
  raw_.add(RawMachine().id(0)).add(RawFairSet().id(0).add(0)).add(RawFairSet().id(1).add(0));
  EXPECT_THROW({
    Situation(raw_, GetParam());
  }, std::invalid_argument);
}

// Verify that Situation constructor throws when a machine has more than one job assigned.
TEST_P(SituationThrowTest, MachineWithMultipleJobs) {
  raw_.add(RawMachine().id(0)).add(RawMachineSet().id(0).add(0));
  raw_.add(RawAccount().id(0)).add(RawBatch().id(0).account(0));
  raw_.add(RawJob().id(1).batch(0).machine_set(0).machine(0));
  raw_.add(RawJob().id(2).batch(0).machine_set(0).machine(0));
  EXPECT_THROW({
    Situation(raw_, GetParam());
  }, std::invalid_argument);
}

// Verify that object with default id doesn't raise an exception when safe == false.
TEST(SituationTest, DefaultId) {
  EXPECT_NO_THROW({
    Situation(RawSituation().add(RawAccount()), false);
  });
  EXPECT_NO_THROW({
    Situation(RawSituation().add(RawMachine()), false);
  });
}

// Verify that the optional relations are actually optional.
TEST_P(SituationThrowTest, OptionalRelations) {
  raw_.add(RawAccount().id(0)).add(RawBatch().id(0).account(0));
  raw_.add(RawMachineSet().id(0).add(0));
  raw_.add(RawJob().id(0).batch(0).machine_set(0));
  EXPECT_NO_THROW({
    Situation(raw_, GetParam());
  });
}

// Verify that null object is returned when some object has missing relation and safe == false.
TEST(SituationTest, MissingRelationIsOk) {
  {
    Situation s(RawSituation().add(RawMachine().id(0)), false);
    Machine m = s[Id<Machine>(0)];
    if (m.fair_set()) ADD_FAILURE();
    if (m.job()) ADD_FAILURE();
  }
  {
    Situation s(RawSituation().add(RawBatch().id(0)), false);
    Batch b = s[Id<Batch>(0)];
    if (b.account()) ADD_FAILURE();
  }
  {
    Situation s(RawSituation().add(RawJob().id(0)), false);
    Job j = s[Id<Job>(0)];
    if (j.machine()) ADD_FAILURE();
    if (j.machine_set()) ADD_FAILURE();
    if (j.batch()) ADD_FAILURE();
  }
}

// Verify that ChangeCosts are correctly copied to the situation.
TEST(SituationTest, ChangeCosts) {
  RawSituation raw;
  raw.change_costs_ = sample.change_costs_;
  Situation s(raw, true);

  for (auto change_cost : raw.change_costs_) {
    EXPECT_EQ(change_cost.cost_, s.change_costs().cost(change_cost.change_));
  }
}

// Verify that costs default to 0 when they are missing and safe == false.
TEST(SituationTest, MissingCosts) {
  Situation s{RawSituation(), false};
  EXPECT_EQ(0, s.change_costs().cost(Change(0, 0, 0)));
  EXPECT_EQ(0, s.change_costs().cost(Change(0, 0, 1)));
  EXPECT_EQ(0, s.change_costs().cost(Change(0, 1, 0)));
  EXPECT_EQ(0, s.change_costs().cost(Change(0, 1, 1)));
  EXPECT_EQ(0, s.change_costs().cost(Change(1, 0, 0)));
  EXPECT_EQ(0, s.change_costs().cost(Change(1, 0, 1)));
  EXPECT_EQ(0, s.change_costs().cost(Change(1, 1, 0)));
  EXPECT_EQ(0, s.change_costs().cost(Change(1, 1, 1)));
}

}  // namespace
}  // namespace lss

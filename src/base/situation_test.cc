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

class SituationTest : public testing::Test {
 protected:
  RawSituation raw_ = RawSituation()
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
      .add(RawBatch().id(22).job_reward(23).job_timely_reward(24).reward(25).timely_reward(26)
          .duration(27).due(28).account(18))
      .add(RawBatch().id(29).job_reward(30).job_timely_reward(31).reward(32).timely_reward(33)
          .duration(34).due(35).account(18))
      .add(RawJob().id(36).duration(37).context(Context(38, 39, 40)).start_time(41)
          .machine(2).machine_set(14).batch(22))
      .add(RawJob().id(42).duration(43).context(Context(44, 45, 46)).start_time(47)
          .machine(10).machine_set(15).batch(22))
      .add(RawChangeCost().change(Change(0, 0, 0)).cost(48))
      .add(RawChangeCost().change(Change(0, 0, 1)).cost(49))
      .add(RawChangeCost().change(Change(0, 1, 0)).cost(50))
      .add(RawChangeCost().change(Change(0, 1, 1)).cost(51))
      .add(RawChangeCost().change(Change(1, 0, 0)).cost(52))
      .add(RawChangeCost().change(Change(1, 0, 1)).cost(53))
      .add(RawChangeCost().change(Change(1, 1, 0)).cost(54))
      .add(RawChangeCost().change(Change(1, 1, 1)).cost(55));
};

// Verify that no object is missing from Situation.
TEST_F(SituationTest, NothingMissing) {
  Situation s{raw_};
  EXPECT_TRUE(IdEqual(raw_.machines_, s.machines()));
  EXPECT_TRUE(IdEqual(raw_.machine_sets_, s.machine_sets()));
  EXPECT_TRUE(IdEqual(raw_.fair_sets_, s.fair_sets()));
  EXPECT_TRUE(IdEqual(raw_.accounts_, s.accounts()));
  EXPECT_TRUE(IdEqual(raw_.batches_, s.batches()));
  EXPECT_TRUE(IdEqual(raw_.jobs_, s.jobs()));
}

// Verify that collections returned by Situation are correctly sorted.
TEST_F(SituationTest, SituationVectorsSorted) {
  // Break order to ensure the elements are actually rearranged.
  RawSituation copy = raw_;
  std::swap(copy.machines_[0], copy.machines_[2]);
  std::swap(copy.machine_sets_[0], copy.machine_sets_[1]);
  std::swap(copy.fair_sets_[0], copy.fair_sets_[1]);
  std::swap(copy.accounts_[0], copy.accounts_[1]);
  std::swap(copy.batches_[0], copy.batches_[1]);
  std::swap(copy.jobs_[0], copy.jobs_[1]);

  Situation s(copy);
  EXPECT_TRUE(IsSorted(s.machines()));
  EXPECT_TRUE(IsSorted(s.machine_sets()));
  EXPECT_TRUE(IsSorted(s.fair_sets()));
  EXPECT_TRUE(IsSorted(s.accounts()));
  EXPECT_TRUE(IsSorted(s.batches()));
  EXPECT_TRUE(IsSorted(s.jobs()));
}

// Verify that collections returned by objects are correctly sorted.
TEST_F(SituationTest, ObjectVectorsSorted) {
  Situation situation{raw_};
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

class SituationTestPairwise : public SituationTest {
 protected:
  Situation situation_{raw_};

  const RawMachine &rm_ = raw_.machines_[1];
  Machine m_ = situation_.machines()[1];
  const RawMachineSet &rs_ = raw_.machine_sets_[0];
  MachineSet s_ = situation_.machine_sets()[0];
  const RawFairSet &rf_ = raw_.fair_sets_[1];
  FairSet f_ = situation_.fair_sets()[1];
  const RawAccount &ra_ = raw_.accounts_[0];
  Account a_ = situation_.accounts()[0];
  const RawBatch &rb_ = raw_.batches_[1];
  Batch b_ = situation_.batches()[1];
  const RawJob &rj_ = raw_.jobs_[0];
  Job j_ = situation_.jobs()[0];
};

// Verify that all "property" fields are correctly copied to Situation.
TEST_F(SituationTestPairwise, Properties) {
  EXPECT_EQ(raw_.time_stamp_, situation_.time_stamp());

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
TEST_F(SituationTestPairwise, FordwardRelations) {
  EXPECT_TRUE(IdEqual(rs_.machines_, s_.machines()));
  EXPECT_TRUE(IdEqual(rf_.machines_, f_.machines()));
  EXPECT_EQ(Id<Account>(rb_.account_), b_.account().id());
  EXPECT_EQ(Id<Machine>(rj_.machine_), j_.machine().id());
  EXPECT_EQ(Id<MachineSet>(rj_.machine_set_), j_.machine_set().id());
  EXPECT_EQ(Id<Batch>(rj_.batch_), j_.batch().id());
}

// Verify that backward relations are correctly represented in situation.
TEST_F(SituationTestPairwise, BackwardRelations) {
  EXPECT_TRUE(Contains(s_, s_.machines().front().machine_sets()));
  EXPECT_EQ(f_.id(), f_.machines().front().fair_set().id());
  EXPECT_EQ(j_.id(), j_.machine().job().id());
  EXPECT_TRUE(Contains(j_, j_.machine_set().jobs()));
  EXPECT_TRUE(Contains(b_, b_.account().batches()));
  EXPECT_TRUE(Contains(j_, j_.batch().jobs()));
}

// Verify that objects are accessible by their id.
TEST_F(SituationTest, AccessById) {
  Situation situation(raw_);

  for (auto &m : raw_.machines_)
    if (!situation[Id<Machine>(m.id_)]) ADD_FAILURE();
  for (auto &s : raw_.machine_sets_)
    if (!situation[Id<MachineSet>(s.id_)]) ADD_FAILURE();
  for (auto &f : raw_.fair_sets_)
    if (!situation[Id<FairSet>(f.id_)]) ADD_FAILURE();
  for (auto &a : raw_.accounts_)
    if (!situation[Id<Account>(a.id_)]) ADD_FAILURE();
  for (auto &b : raw_.batches_)
    if (!situation[Id<Batch>(b.id_)]) ADD_FAILURE();
  for (auto &j : raw_.jobs_)
    if (!situation[Id<Job>(j.id_)]) ADD_FAILURE();
}


// TODO(kzyla): Write a test for each of the following cases:

// Verify that ChangeCosts are correctly copied to the situation.

// Verify that Situation constructor throws when multiple objects have the same id.
// Verify that Situation constructor throws when object has unknown id and safe == true.

// Verify that Situation constructor throws when object has missing relation and safe == true.
// Verify that null-object is returned when some object has missing relation.

// Verify that Situation constructor throws when multiple costs are given for one change.
// Verify that Situation constructor throws when costs are missing and safe == true.
// Verify that costs default to 0 when they are missing.

}  // namespace
}  // namespace lss

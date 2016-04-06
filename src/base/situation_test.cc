#include "base/situation.h"

#include <gtest/gtest.h>

namespace lss {

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

// Verify that forward relations are correctly represented in situation.
TEST_F(SituationTest, FordwardRelations) {
  Situation situation(raw_);


}

// Verify that all "property" fields are correctly copied to Situation.
TEST_F(SituationTest, Properties) {
  Situation situation(raw_);

  const RawMachine &rm = raw_.machines_[1];
  Machine m = situation.machines()[1];
  EXPECT_EQ(Id<Machine>(rm.id_), m.id());
  EXPECT_EQ(rm.state_, m.state());
  EXPECT_EQ(rm.context_, m.context());

  const RawMachineSet &rs = raw_.machine_sets_[0];
  MachineSet s = situation.machine_sets()[0];
  EXPECT_EQ(Id<MachineSet>(rs.id_), s.id());

  const RawFairSet &rf = raw_.fair_sets_[1];
  FairSet f = situation.fair_sets()[1];
  EXPECT_EQ(Id<FairSet>(rf.id_), f.id());

  const RawAccount &ra = raw_.accounts_[0];
  Account a = situation.accounts()[0];
  EXPECT_EQ(Id<Account>(ra.id_), a.id());
  EXPECT_EQ(ra.alloc_, a.alloc());

  const RawBatch &rb = raw_.batches_[1];
  Batch b = situation.batches()[1];
  EXPECT_EQ(Id<Batch>(rb.id_), b.id());
  EXPECT_EQ(rb.job_reward_, b.job_reward());
  EXPECT_EQ(rb.job_timely_reward_, b.job_timely_reward());
  EXPECT_EQ(rb.reward_, b.reward());
  EXPECT_EQ(rb.timely_reward_, b.timely_reward());
  EXPECT_EQ(rb.duration_, b.duration());
  EXPECT_EQ(rb.due_, b.due());

  const RawJob &rj = raw_.jobs_[0];
  Job j = situation.jobs()[0];
  EXPECT_EQ(Id<Job>(rj.id_), j.id());
  EXPECT_EQ(rj.duration_, j.duration());
  EXPECT_EQ(rj.context_, j.context());
  EXPECT_EQ(rj.start_time_, j.start_time());
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

// Verify that Situation constructor throws when multiple objects have the same id.
// Verify that Situation constructor throws when object has unknown id and safe == true.

// Verify that Situation constructor throws when object has missing relation and safe == true.
// Verify that null-object is returned when some object has missing relation.

// Verify that Situation constructor throws when multiple costs are given for one change.
// Verify that Situation constructor throws when costs are missing and safe == true.
// Verify that costs default to 0 when they are missing.

}  // namespace lss

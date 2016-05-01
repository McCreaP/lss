#include "local_search/state.h"

#include "gtest/gtest.h"

#include "base/raw_situation.h"

namespace lss {
namespace local_search {
namespace {

const auto sample = RawSituation()
    .add(RawMachine().id(0))
    .add(RawMachineSet().id(0).add(0))
    .add(RawAccount().id(0))
    .add(RawBatch().id(0).account(0).duration(3))
    .add(RawJob().id(0).batch(0).duration(2))
    .add(RawJob().id(1).batch(0).duration(1));

class StateTest : public ::testing::Test {
 protected:
  void SetUp() override { BuildSituation(sample); }

  void BuildSituation(const RawSituation &raw) {
    situation_ = Situation(raw, false);
    m_ = situation_[Id<Machine>(0)];
    j0_ = situation_[Id<Job>(0)];
    j1_ = situation_[Id<Job>(1)];
  }

  Situation situation_;
  Machine m_;
  Job j0_, j1_;
};

// Verify that default constructor, copy constructor and copy assignment work as expected.
TEST_F(StateTest, Constructors) {
  State s1;
  State s2(s1);
  s1 = s2;
}

// Verify that state with no scheduled jobs evaluates to zero.
TEST_F(StateTest, ZeroEvaluation) {
  EXPECT_NEAR(0, State().Evaluate(), 1e-9);
  EXPECT_NEAR(0, State(Situation(RawSituation(), false)).Evaluate(), 1e-9);
}

// Verify that evaluation of state grows when scheduling previously unassigned jobs.
TEST_F(StateTest, EvaluationGrows) {
  auto raw = sample;
  // We want just one job so finishing it is equivalent to finishing the batch.
  raw.jobs_.pop_back();

  auto test = [&]() {
    BuildSituation(raw);
    State state(situation_);
    state.Assign(m_, j0_);
    double old_eval = state.Evaluate();
    state.Assign(m_, j1_);
    double new_eval = state.Evaluate();
    EXPECT_LT(old_eval, new_eval);
  };

  raw.batches_[0] = RawBatch(sample.batches_[0]).job_reward(1);
  test();
  raw.batches_[0] = RawBatch(sample.batches_[0]).job_timely_reward(1);
  test();
  raw.batches_[0] = RawBatch(sample.batches_[0]).reward(1);
  test();
  raw.batches_[0] = RawBatch(sample.batches_[0]).timely_reward(1);
  test();
}

// Verify that reverting State to previous state preserves its evaluation.
TEST_F(StateTest, EveluationReverts) {
  auto raw = sample;
  raw.batches_[0].reward_ = 1;
  BuildSituation(raw);
  State state(situation_);

  state.Assign(m_, j0_);
  double old_eval = state.Evaluate();
  state.Assign(m_, j1_);
  state.Assign(Machine(), j1_);
  double new_eval = state.Evaluate();
  EXPECT_NEAR(old_eval, new_eval, 1e-9);
}

TEST_F(StateTest, ToSchedule) {
  State state(situation_);

  std::vector<Job> expected;
  EXPECT_EQ(expected, state.ToSchedule().GetJobsAssignedToMachine(m_));
  state.Assign(m_, j0_);
  expected.push_back(j0_);
  EXPECT_EQ(expected, state.ToSchedule().GetJobsAssignedToMachine(m_));
  state.Assign(m_, j1_);
  expected.push_back(j1_);
  EXPECT_EQ(expected, state.ToSchedule().GetJobsAssignedToMachine(m_));
}

TEST_F(StateTest, GetMachine) {
  State state(situation_);

  EXPECT_EQ(Machine(), state.GetMachine(j0_));
  state.Assign(m_, j0_);
  EXPECT_EQ(m_, state.GetMachine(j0_));
  state.Assign(Machine(), j0_);
  EXPECT_EQ(Machine(), state.GetMachine(j0_));
}

TEST_F(StateTest, GetPos) {
  State state(situation_);
  state.Assign(m_, j0_);
  EXPECT_EQ(0, state.GetPos(j0_));
  state.Assign(m_, j1_);
  EXPECT_EQ(1, state.GetPos(j1_));
  state.Assign(m_, j1_, 0);
  EXPECT_EQ(0, state.GetPos(j1_));
  EXPECT_EQ(1, state.GetPos(j0_));
}

TEST_F(StateTest, QueueSize) {
  State state(situation_);
  EXPECT_EQ(2, state.QueueSize(Machine()));
  EXPECT_EQ(0, state.QueueSize(m_));
  state.Assign(m_, j0_);
  EXPECT_EQ(1, state.QueueSize(Machine()));
  EXPECT_EQ(1, state.QueueSize(m_));
  state.Assign(m_, j1_);
  EXPECT_EQ(0, state.QueueSize(Machine()));
  EXPECT_EQ(2, state.QueueSize(m_));
}

TEST_F(StateTest, QueueBack) {
  State state(situation_);
  state.Assign(m_, j0_);
  EXPECT_EQ(j0_, state.QueueBack(m_));
  EXPECT_EQ(j1_, state.QueueBack(Machine()));
}

}  // namespace
}  // namespace local_search
}  // namespace lss

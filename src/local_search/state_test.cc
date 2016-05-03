#include "local_search/state.h"

#include <algorithm>

#include "gtest/gtest.h"

#include "base/raw_situation.h"

namespace lss {
namespace local_search {
namespace {

const auto kSample = RawSituation()
    .add(RawMachine().id(0))
    .add(RawMachineSet().id(0).add(0))
    .add(RawAccount().id(0))
    .add(RawBatch().id(0).account(0).duration(4))
    .add(RawBatch().id(1).account(0).duration(3))
    .add(RawJob().id(0).batch(0).duration(2))
    .add(RawJob().id(1).batch(1).duration(1));

class StateTest : public ::testing::Test {
 protected:
  void SetUp() override { BuildSituation(kSample); }

  void BuildSituation(const RawSituation &raw) {
    situation_ = Situation(raw, false);
    machine_ = situation_[Id<Machine>(0)];
    job0_ = situation_[Id<Job>(0)];
    job1_ = situation_[Id<Job>(1)];
  }

  Situation situation_;
  Machine machine_;
  Job job0_, job1_;
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
  auto raw = kSample;
  auto test = [&]() {
    BuildSituation(raw);
    State state(situation_);
    state.Assign(machine_, job0_);
    double old_eval = state.Evaluate();
    state.Assign(machine_, job1_);
    double new_eval = state.Evaluate();
    EXPECT_LT(old_eval, new_eval);
  };

  raw.batches_[0] = RawBatch(kSample.batches_[0]).job_reward(1);
  raw.batches_[1] = RawBatch(kSample.batches_[1]).job_reward(1);
  test();
  raw.batches_[0] = RawBatch(kSample.batches_[0]).job_timely_reward(1);
  raw.batches_[1] = RawBatch(kSample.batches_[1]).job_timely_reward(1);
  test();
  raw.batches_[0] = RawBatch(kSample.batches_[0]).reward(1);
  raw.batches_[1] = RawBatch(kSample.batches_[1]).reward(1);
  test();
  raw.batches_[0] = RawBatch(kSample.batches_[0]).timely_reward(1);
  raw.batches_[1] = RawBatch(kSample.batches_[1]).timely_reward(1);
  test();
}

// Verify that reverting State to previous state preserves its evaluation.
TEST_F(StateTest, EveluationReverts) {
  auto raw = kSample;
  raw.batches_[0].reward_ = 1;
  BuildSituation(raw);
  State state(situation_);

  state.Assign(machine_, job0_);
  double old_eval = state.Evaluate();
  state.Assign(machine_, job1_);
  state.Assign(Machine(), job1_);
  double new_eval = state.Evaluate();
  EXPECT_NEAR(old_eval, new_eval, 1e-9);
}

// Verify that evaluation correctly discerns the best state in a simple case.
TEST_F(StateTest, SimpleEvaluation) {
  auto raw = RawSituation()
      .time_stamp(0)
      .add(RawMachine().id(0))
      .add(RawMachineSet().id(0).add(0))
      .add(RawAccount().id(0))
      .add(RawBatch().id(0).account(0).duration(0.1).timely_reward(1).due(2))
      .add(RawBatch().id(1).account(0).duration(0.1).timely_reward(1).due(3))
      .add(RawBatch().id(2).account(0).duration(0.1).timely_reward(1).due(4))
      .add(RawJob().id(0).batch(0).machine_set(0).duration(1))
      .add(RawJob().id(1).batch(1).machine_set(0).duration(1))
      .add(RawJob().id(2).batch(2).machine_set(0).duration(1));
  Situation situation(raw, false);

  int permutation[3] = {0, 1, 2};
  auto get_eval = [&]() {
    State state(situation);
    for (int i = 0; i < 3; ++i)
      state.Assign(situation[Id<Machine>(0)], situation[Id<Job>(permutation[i])]);
    return state.Evaluate();
  };

  double best_eval = get_eval();
  while (std::next_permutation(permutation, permutation + 3)) {
    EXPECT_LT(get_eval(), best_eval);
  }
}

TEST_F(StateTest, ToSchedule) {
  State state(situation_);

  std::vector<Job> expected;
  EXPECT_EQ(expected, state.ToSchedule().GetJobsAssignedToMachine(machine_));
  state.Assign(machine_, job0_);
  expected.push_back(job0_);
  EXPECT_EQ(expected, state.ToSchedule().GetJobsAssignedToMachine(machine_));
  state.Assign(machine_, job1_);
  expected.push_back(job1_);
  EXPECT_EQ(expected, state.ToSchedule().GetJobsAssignedToMachine(machine_));
}

TEST_F(StateTest, GetMachine) {
  State state(situation_);

  EXPECT_EQ(Machine(), state.GetMachine(job0_));
  state.Assign(machine_, job0_);
  EXPECT_EQ(machine_, state.GetMachine(job0_));
  state.Assign(Machine(), job0_);
  EXPECT_EQ(Machine(), state.GetMachine(job0_));
}

TEST_F(StateTest, GetPos) {
  State state(situation_);
  state.Assign(machine_, job0_);
  EXPECT_EQ(0, state.GetPos(job0_));
  state.Assign(machine_, job1_);
  EXPECT_EQ(1, state.GetPos(job1_));
  state.Assign(machine_, job1_, 0);
  EXPECT_EQ(0, state.GetPos(job1_));
  EXPECT_EQ(1, state.GetPos(job0_));
}

TEST_F(StateTest, QueueSize) {
  State state(situation_);
  EXPECT_EQ(2, state.QueueSize(Machine()));
  EXPECT_EQ(0, state.QueueSize(machine_));
  state.Assign(machine_, job0_);
  EXPECT_EQ(1, state.QueueSize(Machine()));
  EXPECT_EQ(1, state.QueueSize(machine_));
  state.Assign(machine_, job1_);
  EXPECT_EQ(0, state.QueueSize(Machine()));
  EXPECT_EQ(2, state.QueueSize(machine_));
}

TEST_F(StateTest, QueueBack) {
  State state(situation_);
  state.Assign(machine_, job0_);
  EXPECT_EQ(job0_, state.QueueBack(machine_));
  EXPECT_EQ(job1_, state.QueueBack(Machine()));
}

}  // namespace
}  // namespace local_search
}  // namespace lss

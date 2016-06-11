#include "io/assignment_handler.h"

#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "genetic/test_utils.h"

namespace lss {
namespace io {

using ::testing::Test;
using ::testing::Return;

Schedule BuildSchedule(const std::vector<std::vector<int>> &assignments, Situation situation) {
  Schedule schedule(situation);
  for (size_t machine_id = 0; machine_id < assignments.size(); ++machine_id) {
    for (int job_id : assignments[machine_id]) {
      Job job = situation[Id<Job>(job_id)];
      Machine machine = situation[Id<Machine>(machine_id)];
      schedule.AssignJob(machine, job);
    }
  }
  return schedule;
}

class AssignmentsHandlerShould : public Test {
 protected:
  void SetUp() {
    raw_situation_ = genetic::GetSimpleRawSituation(kNumberOfJobs, kNumberOfMachines);
    situation_ = Situation(raw_situation_);
  }

  const int kNumberOfJobs = 4;
  const int kNumberOfMachines = 3;
  RawSituation raw_situation_;
  Situation situation_;
  io::WriterMock writer_;
};

TEST_F(AssignmentsHandlerShould, assign_job_to_machines_wrt_schedule) {
  Schedule schedule = BuildSchedule({{0, 1, 2}, {}, {3}}, situation_);

  EXPECT_CALL(writer_, Assign(0, 0));
  EXPECT_CALL(writer_, Assign(2, 3));

  AssignmentsHandler assignments_handler(&writer_);
  assignments_handler.AdjustAssignments(schedule);
}

TEST_F(AssignmentsHandlerShould, unasign_job_from_previous_machine_if_not_taken) {
  Schedule old_schedule = BuildSchedule({{0}, {1}, {2}}, situation_);
  Schedule new_schedule = BuildSchedule({{2}, {1}, {0}}, situation_);

  EXPECT_CALL(writer_, Assign(0, 0)).WillOnce(Return(true));
  EXPECT_CALL(writer_, Assign(1, 1)).Times(2).WillRepeatedly(Return(true));
  EXPECT_CALL(writer_, Assign(2, 2)).WillOnce(Return(true));
  EXPECT_CALL(writer_, Unassign(0)).WillOnce(Return(true));
  EXPECT_CALL(writer_, Unassign(1)).WillOnce(Return(true));
  EXPECT_CALL(writer_, Unassign(2)).WillOnce(Return(true));

  EXPECT_CALL(writer_, Assign(0, 2));
  EXPECT_CALL(writer_, Assign(2, 0));

  AssignmentsHandler assignments_handler(&writer_);
  assignments_handler.AdjustAssignments(old_schedule);
  assignments_handler.AdjustAssignments(new_schedule);
}

TEST_F(AssignmentsHandlerShould, not_unasign_job_from_previous_machine_if_taken) {
  Schedule old_schedule = BuildSchedule({{0}, {1}, {2}}, situation_);
  Schedule new_schedule = BuildSchedule({{1, 3}, {0}, {2}}, situation_);

  EXPECT_CALL(writer_, Assign(0, 0)).WillOnce(Return(true));
  EXPECT_CALL(writer_, Assign(1, 1)).WillOnce(Return(true));
  EXPECT_CALL(writer_, Assign(2, 2)).WillOnce(Return(true));
  EXPECT_CALL(writer_, Unassign(0)).WillOnce(Return(true));
  EXPECT_CALL(writer_, Unassign(1)).WillOnce(Return(false));
  EXPECT_CALL(writer_, Unassign(2)).WillOnce(Return(false));

  EXPECT_CALL(writer_, Assign(0, 3));
  EXPECT_CALL(writer_, Assign(1, 0));

  AssignmentsHandler assignments_handler(&writer_);
  assignments_handler.AdjustAssignments(old_schedule);
  assignments_handler.AdjustAssignments(new_schedule);
}

TEST_F(AssignmentsHandlerShould, set_empty_context_on_new_machine) {
  AssignmentsHandler assignments_handler(&writer_);
  assignments_handler.AdjustRawSituation(&raw_situation_);
  for (const RawMachine &raw_machine : raw_situation_.machines_) {
    EXPECT_EQ(Context(), raw_machine.context_);
  }
}

TEST_F(AssignmentsHandlerShould, fill_machine_context) {
  const Context kContexts[] = { Context(1, 2, 3), Context(4, 5, 6), Context(7, 8, 9) };
  RawSituation raw_situation = genetic::GetSimpleRawSituation(kNumberOfJobs, kNumberOfMachines);
  for (size_t i = 0; i < raw_situation.jobs_.size(); ++i) {
    raw_situation.jobs_[i].context(kContexts[i]);
  }
  Situation situation(raw_situation);
  Schedule schedule = BuildSchedule({{0}, {1}, {2}}, situation);

  EXPECT_CALL(writer_, Assign(0, 0)).WillOnce(Return(true));
  EXPECT_CALL(writer_, Assign(1, 1)).WillOnce(Return(true));
  EXPECT_CALL(writer_, Assign(2, 2)).WillOnce(Return(true));

  EXPECT_CALL(writer_, DoesAssignmentExist(0)).WillOnce(Return(false));
  EXPECT_CALL(writer_, DoesAssignmentExist(1)).WillOnce(Return(false));
  EXPECT_CALL(writer_, DoesAssignmentExist(2)).WillOnce(Return(true));

  AssignmentsHandler assignments_handler(&writer_);
  assignments_handler.AdjustAssignments(schedule);
  assignments_handler.AdjustRawSituation(&raw_situation);

  EXPECT_EQ(kContexts[0], raw_situation.machines_[0].context_);
  EXPECT_EQ(kContexts[1], raw_situation.machines_[1].context_);
  EXPECT_EQ(Context(), raw_situation.machines_[2].context_);
}

}  // namespace io
}  // namespace lss

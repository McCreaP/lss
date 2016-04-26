#include "base/schedule.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "genetic/test_utils.h"

namespace lss {

using ::testing::Test;
using ::testing::Return;

Schedule BuildSchedule(const std::vector<std::vector<int>> &assignments, Situation situation) {
  Schedule schedule;
  for (size_t machine_id = 0; machine_id < assignments.size(); ++machine_id) {
    for (int job_id : assignments[machine_id]) {
      Job job = situation[Id<Job>(job_id)];
      Machine machine = situation[Id<Machine>(machine_id)];
      schedule.AssignJob(machine, job);
    }
  }
  return schedule;
}

class DriverShould : public Test {
 protected:
  void SetUp() {
    RawSituation rawSituation_ = genetic::GetSimpleRawSituation(kNumberOfJobs, kNumberOfMachines);
    situation_ = Situation(rawSituation_);
  }

  const int kNumberOfJobs = 4;
  const int kNumberOfMachines = 3;
  Situation situation_;
  io::WriterMock writer_;
};

TEST_F(DriverShould, assign_job_to_machines_wrt_schedule) {
  Schedule schedule = BuildSchedule({{0, 1, 2}, {}, {3}}, situation_);

  EXPECT_CALL(writer_, Assign(0, 0));
  EXPECT_CALL(writer_, Assign(2, 3));

  AssignmentsHandler driver(&writer_);
  driver.AdjustAssignments(schedule, situation_);
}

TEST_F(DriverShould, unasigned_job_from_previous_machine_if_not_taken) {
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

  AssignmentsHandler driver(&writer_);
  driver.AdjustAssignments(old_schedule, situation_);
  driver.AdjustAssignments(new_schedule, situation_);
}

TEST_F(DriverShould, not_unasigned_job_from_previous_machine_if_taken) {
  Schedule old_schedule = BuildSchedule({{0}, {1}, {2}}, situation_);
  Schedule new_schedule = BuildSchedule({{1, 3}, {0}, {2}}, situation_);

  EXPECT_CALL(writer_, Assign(0, 0)).WillOnce(Return(true));
  EXPECT_CALL(writer_, Assign(1, 1)).WillOnce(Return(true));
  EXPECT_CALL(writer_, Assign(2, 2)).WillOnce(Return(true));
  // Jobs 1 and 2 will have been consumed by the next driver call.
  EXPECT_CALL(writer_, Unassign(0)).WillOnce(Return(true));
  EXPECT_CALL(writer_, Unassign(1)).WillOnce(Return(false));
  EXPECT_CALL(writer_, Unassign(2)).WillOnce(Return(false));

  EXPECT_CALL(writer_, Assign(0, 3));
  EXPECT_CALL(writer_, Assign(1, 0));

  AssignmentsHandler driver(&writer_);
  driver.AdjustAssignments(old_schedule, situation_);
  driver.AdjustAssignments(new_schedule, situation_);
}

TEST_F(DriverShould, adopt_to_new_situation) {}

}  // namespace lss

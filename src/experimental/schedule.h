#ifndef LSS_EXPERIMENTAL_SCHEDULE_H_
#define LSS_EXPERIMENTAL_SCHEDULE_H_

class Schedule {
 public:
  Schedule();
  Schedule(const Schedule &old_schedule, const Situation &new_situation);
};

#endif  // LSS_EXPERIMENTAL_SCHEDULE_H_

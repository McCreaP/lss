#include "base/schedule.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <set>
#include <unordered_set>
#include <glog/logging.h>

#include "base/situation.h"

namespace lss {
namespace {

using JobFinishTime = std::unordered_map<Job, Time>;

double Sigmoid(double reward, double timely_reward, Time time) {
  return reward + timely_reward / (1 + exp(time));
}

Time NormalizeTime(Batch batch, Time time) {
  return (time - batch.due()) / batch.duration();
}

double JobReward(Job job, Time time) {
  Batch batch = job.batch();
  return Sigmoid(batch.job_reward(), batch.job_timely_reward(), NormalizeTime(batch, time));
}

double BatchReward(Batch batch, Time time) {
  return Sigmoid(batch.reward(), batch.timely_reward(), NormalizeTime(batch, time));
}

double ChangeCost(const Situation &situation, Job from, Job to) {
  return situation.change_costs().cost(from.context(), to.context());
}

double JobsIngredient(const Schedule &schedule,
                      Situation situation,
                      JobFinishTime *job_finish_time) {
  double result = 0.;
  for (const auto &assignment : schedule.GetAssignments()) {
    Time time = situation.time_stamp();
    auto jobs = assignment.second;
    for (size_t i = 0; i < jobs.size(); ++i) {
      Job job = jobs[i];
      double change_cost = (i ? ChangeCost(situation, jobs[i - 1], job) : 0);
      time += job.duration() + change_cost;
      (*job_finish_time)[job] = time;
      result += JobReward(job, time);
    }
  }
  return result;
}

double BatchIngredient(Situation situation, const JobFinishTime &job_finish_time) {
  double result = 0.;
  for (Batch batch : situation.batches()) {
    Time batch_finish_time = std::numeric_limits<Time>::min();
    for (Job job : batch.jobs()) {
      if (job_finish_time.count(job)) {
        batch_finish_time = std::max(batch_finish_time, job_finish_time.at(job));
      }
    }
    if (batch_finish_time != std::numeric_limits<int>::min()) {
      result += BatchReward(batch, batch_finish_time);
    }
  }
  return result;
}

}  // namespace

double ObjectiveFunction(const Schedule &schedule, Situation situation) {
  JobFinishTime job_finish_time;
  double result = JobsIngredient(schedule, situation, &job_finish_time);
  result += BatchIngredient(situation, job_finish_time);
  return result;
}

}  // namespace lss

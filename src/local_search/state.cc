#include "local_search/state.h"

#include <cmath>
#include <exception>

#include "glog/logging.h"

namespace lss {
namespace local_search {

State::State(Situation s) : situation_(s) {
  queue_[Machine()];
  for (auto m : situation_.machines()) queue_[m];
  for (auto j : situation_.jobs()) {
    queue_[Machine()].push_back(Entry(j));
    assignment_[j] = Machine();
  }

  for (auto j : situation_.jobs())
    if (!j.batch()) throw std::invalid_argument("All jobs must have a batch set.");
  for (auto b : situation_.batches())
    if (!b.account()) throw std::invalid_argument("All batches must have an account set.");
}

Schedule State::ToSchedule() const {
  Schedule schedule(situation_);
  for (auto key_val : queue_)
    for (auto entry : key_val.second)
      schedule.AssignJob(entry.job, key_val.first);
  return schedule;
}

Machine State::GetMachine(Job j) const {
  auto it = assignment_.find(j);
  if (it == assignment_.end()) throw std::invalid_argument("Invalid job.");
  return it->second;
}

size_t State::GetPos(Job j) const {
  auto it = assignment_.find(j);
  if (it == assignment_.end()) throw std::invalid_argument("Invalid job.");
  return JobPos(queue_.at(it->second), j);
}

size_t State::QueueSize(Machine m) const {
  auto it = queue_.find(m);
  if (it == queue_.end()) throw std::invalid_argument("Invalid machine.");
  return it->second.size();
}

Job State::QueueBack(Machine m) const {
  auto it = queue_.find(m);
  if (it == queue_.end()) throw std::invalid_argument("Invalid machine.");
  return it->second.back().job;
}

void State::Assign(Machine new_machine, Job job, size_t new_pos) {
  auto new_queue_it = queue_.find(new_machine);
  if (new_queue_it == queue_.end()) throw std::invalid_argument("Invalid machine.");
  auto assignment_it = assignment_.find(job);
  if (assignment_it == assignment_.end()) throw std::invalid_argument("Invalid job.");

  auto &assignment = assignment_it->second;
  auto &old_queue = queue_.at(assignment);
  auto &new_queue = new_queue_it->second;

  size_t old_pos = JobPos(old_queue, job);
  eval_ -= old_queue[old_pos].eval_contribution;
  old_queue.erase(old_queue.begin() + old_pos);
  RecomputeTail(&old_queue, old_pos);

  new_pos = std::min(new_pos, new_queue.size());
  new_queue.insert(new_queue.begin() + new_pos, Entry(job));
  RecomputeTail(&new_queue, new_pos);

  assignment = new_machine;
}

double State::JobEval(Job j, Time finish_time) const {
  Batch b = j.batch();

  double r = (finish_time - j.batch().due()) / b.duration();
  double job_reward = b.job_reward() + b.job_timely_reward() / (1 + exp(r));
  double batch_reward = b.reward() + b.timely_reward() / (1 + exp(r));

  return job_reward + batch_reward / b.jobs().size();
}

Time State::EstimatedStartTime(const MachineQueue &queue, size_t pos) const {
  // TODO: Take into account the job currently being executed by given machine.
  if (pos == 0) return situation_.time_stamp();
  else return queue[pos - 1].finish_time;
}

void State::RecomputeTail(MachineQueue *queue, size_t pos) {
  CHECK_NOTNULL(queue);
  // Unassigned jobs are not taken into account by `Evaluate()`.
  if (queue == &queue_.at(Machine())) return;

  Time time = EstimatedStartTime(*queue, pos);
  for (size_t i = pos; i < queue->size(); ++i) {
    time += (*queue)[i].job.duration();
    (*queue)[i].finish_time = time;

    eval_ -= (*queue)[i].eval_contribution;
    (*queue)[i].eval_contribution = JobEval((*queue)[i].job, time);
    eval_ += (*queue)[i].eval_contribution;
  }
}

size_t State::JobPos(const MachineQueue &queue, Job j) const {
  for (auto it = queue.end() - 1; it >= queue.begin(); --it)
    if (it->job == j) return it - queue.begin();
  LOG(FATAL) << "JobPos(queue, j): job not found in queue.";
}

}  // namespace local_search
}  // namespace lss

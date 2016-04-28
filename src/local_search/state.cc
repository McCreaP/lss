#include "local_search/state.h"

#include <cmath>

namespace lss {
namespace local_search {

State::State(Situation s) : situation_(s) {
  queue_[Machine()];
  for (auto m : situation_.machines()) queue_[m];
  for (auto j : situation_.jobs()) {
    queue_[Machine()].push_back(Entry(j));
    job_machine_[j] = Machine();
  }
}

Schedule State::ToSchedule() const {
  Schedule schedule(situation_);
  for (auto key_val : queue_)
    for (auto entry : key_val.second)
      schedule.AssignJob(entry.job, key_val.first);
  return schedule;
}

Machine State::GetMachine(Job j) const {
  auto it = job_machine_.find(j);
  if (it != job_machine_.end())
    return it->second;
  return Machine();
}

size_t State::GetPos(Job j) const {
  auto it = job_machine_.find(j);
  if (it == job_machine_.end())
    return std::numeric_limits<size_t>::max();

  Machine m = it->second;
  auto schedule = queue_.at(m);
  for (size_t pos = schedule.size() - 1; ; --pos) {
    if (schedule[pos].job == j)
      return pos;
  }
}

size_t State::QueueSize(Machine m) const {
  auto it = queue_.find(m);
  if (it != queue_.end())
    return it->second.size();
  return 0;
}

Job State::QueueBack(Machine m) const {
  auto it = queue_.find(m);
  if (it != queue_.end())
    return it->second.back().job;
  return Job();
}

void State::Assign(Machine m, Job j, size_t pos) {
  if (queue_.find(m) == queue_.end() || job_machine_.find(j) == job_machine_.end())
    return;

  auto old_queue = queue_.at(job_machine_.at(j));
  auto old_it = FindJob(&old_queue, j);
  eval_ -= old_it->eval;
  old_queue.erase(old_it);
  RecomputeTail(job_machine_.at(j), old_it - old_queue.begin());

  auto new_queue = queue_.at(m);
  auto new_it = std::min(new_queue.end(), new_queue.begin() + pos);
  new_queue.insert(new_it, Entry(j));
  RecomputeTail(m, new_it - new_queue.begin());

  job_machine_.at(j) = m;
}

double State::JobEval(Job j, Time start_time) {
  Batch b = j.batch();

  Time complete_time = start_time + j.duration();
  double r = (complete_time - j.batch().due()) / b.duration();

  double job_reward = b.job_timely_reward() + b.job_reward() / (1 + exp(r));
  double batch_reward = b.timely_reward() + b.reward() / (1 + exp(r));
  return job_reward + batch_reward / b.jobs().size();
}

void State::RecomputeTail(Machine m, size_t pos) {
  if (!m) return;

  auto queue = queue_.at(m);
  Time time = pos == 0 ? situation_.time_stamp() : queue[pos - 1].finish_time;
  for (size_t i = pos; i < queue.size(); ++i) {
    eval_ -= queue[i].eval;
    queue[i].eval = JobEval(queue[i].job, time);
    eval_ += queue[i].eval;
    time += queue[i].job.duration();
    queue[i].finish_time = time;
  }
}

State::MachineQueue::iterator State::FindJob(MachineQueue *queue, Job j) {
  for (auto it = queue->end() - 1; ; --it)
    if (it->job == j) return it;
}

}  // namespace local_search
}  // namespace lss

#include <istream>
#include <sstream>
#include "base/raw_situation.h"

namespace lss {
namespace io {

std::istream &operator>>(std::istream &input, ::lss::RawJob &job) {
  input >> job.id_ >> job.batch_ >> job.duration_ >> job.machine_set_;
  for (int i = 0; i < ::lss::Context::kSize; ++i)
    input >> job.context_[i];
  return input;
}

std::istream &operator>>(std::istream &input, ::lss::RawBatch &batch) {
  input >> batch.id_ >> batch.account_;
  input >> batch.job_reward_ >> batch.job_timely_reward_ >> batch.reward_ >> batch.timely_reward_;
  input >> batch.duration_ >> batch.due_;
  return input;
}

std::istream &operator>>(std::istream &input, ::lss::RawMachine &machine) {
  int state;
  input >> machine.id_ >> state;
  machine.state_ = static_cast<::lss::MachineState>(state);
  return input;
}

std::istream &operator>>(std::istream &input, ::lss::RawMachineSet &set) {
  input >> set.id_;

  std::string line;
  std::getline(input, line);
  std::istringstream line_stream(line);
  int machine_id;
  while (line_stream >> machine_id) set.machines_.push_back(machine_id);

  return input;
}

std::istream &operator>>(std::istream &input, ::lss::RawAccount &account) {
  input >> account.id_ >> account.alloc_;
  return input;
}

std::istream &operator>>(std::istream &input, ::lss::RawChangeCost &change) {
  for (int i = 0; i < ::lss::Context::kSize; ++i)
    input >> change.change_[i];
  input >> change.cost_;
  return input;
}

}  // namespace io
}  // namespace lss

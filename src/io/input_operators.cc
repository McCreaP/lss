#include <istream>
#include <sstream>
#include "io/raw_input_types.h"

namespace lss {
namespace io {

std::istream &operator>>(std::istream &input, Job &job) {
  input >> job.id >> job.batch_id >> job.duration >> job.machineset_id;
  for (int i = 0; i < kContextN; ++i)
    input >> job.context[i];
  return input;
}

std::istream &operator>>(std::istream &input, Batch &batch) {
  input >> batch.id >> batch.account_id;
  input >> batch.timely_reward >> batch.reward >> batch.expected_time;
  input >> batch.due;
  return input;
}

std::istream &operator>>(std::istream &input, Machine &machine) {
  int state;
  input >> machine.id >> state;
  machine.state = static_cast<MachineState>(state);
  return input;
}

std::istream &operator>>(std::istream &input, MachineSet &set) {
  input >> set.id;

  std::string line;
  std::getline(input, line);
  std::istringstream line_stream(line);
  int machine_id;
  while (line_stream >> machine_id) set.machines.push_back(machine_id);

  return input;
}

std::istream &operator>>(std::istream &input, Account &account) {
  input >> account.id >> account.alloc;
  return input;
}

std::istream &operator>>(std::istream &input, ContextChange &change) {
  for (int i = 0; i < kContextN; ++i)
    input >> change.changed[i];
  input >> change.cost;
  return input;
}

}  // namespace io
}  // namespace lss

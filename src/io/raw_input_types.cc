#include "io/raw_input_types.h"

#include <iostream>

namespace lss {
namespace io {

Job& Job::operator=(const Job& rhs) {
  id = rhs.id;
  machineset_id = rhs.machineset_id;
  batch_id = rhs.batch_id;
  duration = rhs.duration;
  for (int i = 0; i < kContextN; ++i)
    context[i] = rhs.context[i];
  return *this;
}

bool Job::operator==(const Job& rhs) const {
  bool ok = true;
  ok &= (id == rhs.id);
  ok &= (machineset_id == rhs.machineset_id);
  ok &= (batch_id == rhs.batch_id);
  ok &= (duration == rhs.duration);
  for (int i = 0; i < kContextN; ++i)
    ok &= (context[i] == rhs.context[i]);
  return ok;
}

bool Batch::operator==(const Batch& rhs) const {
  return id == rhs.id &&
      account_id == rhs.account_id &&
      timely_reward == rhs.timely_reward &&
      reward == rhs.reward &&
      expected_time == rhs.expected_time &&
      due == rhs.due;
}

}  // namespace io
}  // namespace lss

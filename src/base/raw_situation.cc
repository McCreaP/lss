#include "base/raw_situation.h"

#include <iostream>

namespace lss {

bool RawJob::operator==(const RawJob& rhs) const {
  bool ok = true;
  ok &= (id_ == rhs.id_);
  ok &= (machine_set_ == rhs.machine_set_);
  ok &= (batch_ == rhs.batch_);
  ok &= (duration_ == rhs.duration_);
  for (int i = 0; i < Context::kSize; ++i)
    ok &= (context_[i] == rhs.context_[i]);
  return ok;
}

bool RawBatch::operator==(const RawBatch& rhs) const {
  return id_ == rhs.id_ &&
      account_ == rhs.account_ &&
      timely_reward_ == rhs.timely_reward_ &&
      reward_ == rhs.reward_ &&
      duration_ == rhs.duration_ &&
      due_ == rhs.due_;
}

}  // namespace lss

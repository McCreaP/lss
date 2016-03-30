#include "base/situation.h"

namespace lss {

Situation::~Situation() {
  for (auto m : machines_) delete m.data_;
  for (auto s : machine_sets_) delete s.data_;
  for (auto f : fair_sets_) delete f.data_;
  for (auto a : accounts_) delete a.data_;
  for (auto b : batches_) delete b.data_;
  for (auto j : jobs_) delete j.data_;
}

}  // namespace lss

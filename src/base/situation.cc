#include "base/situation.h"

#include <algorithm>

namespace lss {
namespace {

template<class T>
bool IdCmp(T lhs, T rhs) { return lhs.id() < rhs.id(); }

}  // namespace

Situation::~Situation() {
  for (auto m : machines_) delete m.data_;
  for (auto s : machine_sets_) delete s.data_;
  for (auto f : fair_sets_) delete f.data_;
  for (auto a : accounts_) delete a.data_;
  for (auto b : batches_) delete b.data_;
  for (auto j : jobs_) delete j.data_;
}

template<class T>
T Situation::Get(const std::vector<T> &from, Id<T> id) {
  // We'd use lower_bound but it would require creating special dummy object for holding id.
  auto lo = from.begin(), hi = from.end();
  while (hi - lo > 1) {
    auto mid = lo + (hi - lo) / 2;
    if (id < mid->id())
      hi = mid;
    else
      lo = mid;
  }

  if (lo == hi || lo->id() != id)
    return T();
  else
    return *lo;
}

}  // namespace lss

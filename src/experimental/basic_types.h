#ifndef LSS_EXPERIMENTAL_BASIC_TYPES_H_
#define LSS_EXPERIMENTAL_BASIC_TYPES_H_

#include <bitset>
#include <climits>

namespace lss {

static constexpr int kContextN = 3;

// The definitions for the following should be moved to this file:
enum class MachineState;

// More adequate types might be substituted in the future.
using FloatType = double;
using Time = double;
using Duration = double;
using Cost = int;

template<T>
class Id {
 public:
  static constexpr int kIdNone = std::numeric_limits<int>::min();

  Id() = default;
  Id(const Id &) = default;
  Id& operator=(const Id &) = default;

  explicit Id(int id) : id_(id) {}

  explicit operator int() const { return id_; }
  int ToInt() const { return id_; }

  static Id None() { return Id(); }

  friend bool operator==(Id lhs, Id rhs) { return lhs.id_ == rhs.id_; }
  friend bool operator!=(Id lhs, Id rhs) { return lhs.Id != rhs.id_; }
  friend bool operator<(Id lhs, Id rhs) { return lhs.id_ < rhs.id_; }
  friend bool operator>(Id lhs, Id rhs) { return lhs.id_ > rhs.id_; }
  friend bool operator<=(Id lhs, Id rhs) { return lhs.id_ <= rhs.id_; }
  friend bool operator>=(Id lhs, Id rhs) { return lhs.id_ >= rhs.id_; }

 private:
  const int id_ = kIdNone;
};

class Context {
 public:
  Context() = default;
  Context(const Context &) = default;
  Context& operator=(const Context &) = default;

  Context(int c1, int c2, int c3) : context_({c1, c2, c3}) {}

  int Get(size_t idx) const { return context_[idx]; }
  Context& Set(size_t idx, int val) { context_[idx] = val; return *this; }

  static Context None() { return Context(); }

  friend bool operator==(const Context &lhs, const Context &rhs) { return lhs.context_ == rhs.context_; }
  friend bool operator!=(const Context &lhs, const Context &rhs) { return lhs.context_ != rhs.context_; }
  friend bool operator<(const Context &lhs, const Context &rhs) { return lhs.context_ < rhs.context_; }
  friend bool operator>(const Context &lhs, const Context &rhs) { return lhs.context_ > rhs.context_; }
  friend bool operator<=(const Context &lhs, const Context &rhs) { return lhs.context_ <= rhs.context_; }
  friend bool operator>=(const Context &lhs, const Context &rhs) { return lhs.context_ >= rhs.context_; }

 private:
  static constexpr int kContextNone = std::numeric_limits<int>::min();

  int context_[kContextN] = {kContextNone, kContextNone, kContextNone};
};

class Change {
 public:
  Change() = default;
  Change(const Change &) = default;
  Change& operator=(const Change &) = default;

  Change(bool c1, bool c2, bool c3) : change_({c1, c2, c3, true}) {}
  Change(const Context &before, const Context &after) {
    change_.set(kContextN);
    for (int i = 0; i < kContextN; ++i)
      change_.set(i, before.Get(i) != after.Get(i));
  }

  bool Get(size_t idx) const { return change_.test(idx); }
  Change& Set(size_t idx, bool val) { change_.set(idx, val); return *this; }

  static Change None() { return Change(); }

  friend bool operator==(const Change &lhs, const Change &rhs) { return lhs.change_ == rhs.change_; }
  friend bool operator!=(const Change &lhs, const Change &rhs) { return lhs.change_ != rhs.change_; }
  friend bool operator<(const Change &lhs, const Change &rhs) { return lhs.change_ < rhs.change_; }
  friend bool operator>(const Change &lhs, const Change &rhs) { return lhs.change_ > rhs.change_; }
  friend bool operator<=(const Change &lhs, const Change &rhs) { return lhs.change_ <= rhs.change_; }
  friend bool operator>=(const Change &lhs, const Change &rhs) { return lhs.change_ >= rhs.change_; }

 private:
  // The additional bit allows us to represent unknown change.
  std::bitset<kContextN + 1> change_;
};

}  // namespace lss

namespace std {

template<class T>
struct hash<lss::Id<T>> {
  size_t operator()(const Id<T> &x) const { return hash<int>()(static_cast<int>(x)); }
};

template<>
struct hash<lss::Context> {
  size_t operator()(const Context &x) const {
    // TODO: Use boost::hash_combine instead.
    return hash<int>()(x.Get(0) ^ 0xDeadBeef) ^ hash<int>()(x.Get(1) ^ 0xBeefDead)
        ^ hash<int>()(x.Get(2) ^ 0xCafeBabe);
  }
};

template<>
struct hash<lss::Change> {

};

}  // namespace std

#endif  // LSS_EXPERIMENTAL_BASIC_TYPES_H_

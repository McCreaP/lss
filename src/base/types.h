#ifndef LSS_BASE_TYPES_H_
#define LSS_BASE_TYPES_H_

#include <bitset>
#include <limits>

namespace lss {

// More adequate types might be substituted in the future.
using FloatType = double;
using Time = double;
using Duration = double;
using Cost = int;

enum class MachineState {
  kIdle = 0,
  kWorking = 1,
  kDead = 2,
};

template<class T>
class Id {
 public:
  static constexpr int kNone = std::numeric_limits<int>::min();

  Id() = default;
  explicit Id(int id) : id_(id) {}

  explicit operator int() const { return id_; }

  friend bool operator==(Id lhs, Id rhs) { return lhs.id_ == rhs.id_; }
  friend bool operator!=(Id lhs, Id rhs) { return lhs.Id != rhs.id_; }
  friend bool operator<(Id lhs, Id rhs) { return lhs.id_ < rhs.id_; }
  friend bool operator>(Id lhs, Id rhs) { return lhs.id_ > rhs.id_; }
  friend bool operator<=(Id lhs, Id rhs) { return lhs.id_ <= rhs.id_; }
  friend bool operator>=(Id lhs, Id rhs) { return lhs.id_ >= rhs.id_; }

 private:
  int id_ = kNone;
};

class Context {
 public:
  static constexpr int kSize = 3;
  static constexpr int kNone = std::numeric_limits<int>::min();

  Context() = default;
  Context(int c1, int c2, int c3) : context_{c1, c2, c3} {}

  int operator[](size_t idx) const { return context_[idx]; }

  friend bool operator==(const Context &lhs, const Context &rhs) { return lhs.context_ == rhs.context_; }
  friend bool operator!=(const Context &lhs, const Context &rhs) { return lhs.context_ != rhs.context_; }
  friend bool operator<(const Context &lhs, const Context &rhs) { return lhs.context_ < rhs.context_; }
  friend bool operator>(const Context &lhs, const Context &rhs) { return lhs.context_ > rhs.context_; }
  friend bool operator<=(const Context &lhs, const Context &rhs) { return lhs.context_ <= rhs.context_; }
  friend bool operator>=(const Context &lhs, const Context &rhs) { return lhs.context_ >= rhs.context_; }

 private:
  int context_[kSize] = {kNone, kNone, kNone};
};

class Change {
 public:
  Change() = default;
  Change(bool c1, bool c2, bool c3) : change_(1*c1 + 2*c2 + 4*c3) {}
  Change(const Context &from, const Context &to)
      : Change(from[0] != to[0], from[1] != to[1], from[2] != to[2]) {}

  bool operator[](size_t idx) const { return change_.test(idx); }

 private:
  std::bitset<Context::kSize> change_;

  friend struct std::hash<Change>;
};

}  // namespace lss

namespace std {

template<class T>
struct hash<lss::Id<T>> {
  size_t operator()(const lss::Id<T> &x) const { return hash<int>()(static_cast<int>(x)); }
};

template<>
struct hash<lss::Context> {
  size_t operator()(const lss::Context &x) const {
    // TODO: Use boost::hash_combine instead.
    return hash<int>()(x[0] ^ 0xDeadBeef) ^ hash<int>()(x[1] ^ 0xBeefDead)
        ^ hash<int>()(x[2] ^ 0xCafeBabe);
  }
};

template<>
struct hash<lss::Change> {
  size_t operator()(const lss::Change &x) const { return hash<bitset<lss::Context::kSize>>()(x.change_); }
};

}  // namespace std

#endif  // LSS_BASE_TYPES_H_

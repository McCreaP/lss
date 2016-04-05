#ifndef LSS_BASE_TYPES_H_
#define LSS_BASE_TYPES_H_

#include <cstdlib>

#include <array>
#include <limits>

namespace lss {

// More adequate types might be substituted in the future.
using IdType = int;
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
  explicit operator bool() const { return id_ != kNone; }

  friend bool operator==(Id lhs, Id rhs) { return lhs.id_ == rhs.id_; }
  friend bool operator!=(Id lhs, Id rhs) { return lhs.id_ != rhs.id_; }
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

  int& operator[](size_t idx) { return context_[idx]; }
  const int& operator[](size_t idx) const { return context_[idx]; }

  friend bool operator==(const Context &lhs, const Context &rhs) {
    return lhs.context_ == rhs.context_;
  }
  friend bool operator!=(const Context &lhs, const Context &rhs) {
    return lhs.context_ != rhs.context_;
  }
  friend bool operator<(const Context &lhs, const Context &rhs) {
    return lhs.context_ < rhs.context_;
  }
  friend bool operator>(const Context &lhs, const Context &rhs) {
    return lhs.context_ > rhs.context_;
  }
  friend bool operator<=(const Context &lhs, const Context &rhs) {
    return lhs.context_ <= rhs.context_;
  }
  friend bool operator>=(const Context &lhs, const Context &rhs) {
    return lhs.context_ >= rhs.context_;
  }

 private:
  std::array<int, kSize> context_{{kNone, kNone, kNone}};
};

class Change {
 public:
  static constexpr int kSize = Context::kSize;
  static constexpr int kNum = 1 << kSize;  // The number of possible changes.

  Change() = default;
  Change(bool c1, bool c2, bool c3) : change_{c1, c2, c3} {}
  Change(const Context &from, const Context &to)
      : Change(from[0] != to[0], from[1] != to[1], from[2] != to[2]) {}

  // Returns a number with corresponding bits.
  explicit operator size_t() const { return 1 * change_[0] + 2 * change_[1] + 4 * change_[2]; }

  bool& operator[](size_t idx) { return change_[idx]; }
  const bool& operator[](size_t idx) const { return change_[idx]; }

 private:
  bool change_[kSize];
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
    // TODO(kzyla): Use boost::hash_combine instead.
    return hash<int>()(x[0] ^ 0xDeadBeef) ^ hash<int>()(x[1] ^ 0xBeefDead)
        ^ hash<int>()(x[2] ^ 0xCafeBabe);
  }
};

template<>
struct hash<lss::Change> {
  size_t operator()(const lss::Change &x) const {
    return hash<size_t>()(static_cast<size_t>(x));
  }
};

}  // namespace std

#endif  // LSS_BASE_TYPES_H_

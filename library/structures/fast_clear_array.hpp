#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <vector>

namespace ht {

template <class T>
class FastClearArray {
  std::vector<T> value_;
  std::vector<std::uint32_t> stamp_;
  std::uint32_t generation_ = 1;
  T default_{};

 public:
  FastClearArray() = default;
  explicit FastClearArray(int n, T default_value = T{})
      : value_(n), stamp_(n, 0), default_(default_value) {}
  int size() const { return static_cast<int>(value_.size()); }
  void clear() {
    if (++generation_ == 0) {
      std::fill(stamp_.begin(), stamp_.end(), 0);
      generation_ = 1;
    }
  }
  bool contains(int i) const { assert(0 <= i && i < size()); return stamp_[i] == generation_; }
  T get(int i) const { return contains(i) ? value_[i] : default_; }
  void set(int i, const T& value) {
    assert(0 <= i && i < size()); stamp_[i] = generation_; value_[i] = value;
  }
};

}  // namespace ht

#pragma once

#include <cassert>
#include <vector>

namespace ht {

// Dense/sparse set for keys [0, universe). O(1) insert/erase/clear.
class FastSet {
  std::vector<int> dense_, sparse_;

 public:
  explicit FastSet(int universe = 0) : sparse_(universe, -1) { dense_.reserve(universe); }
  bool contains(int x) const {
    assert(0 <= x && x < static_cast<int>(sparse_.size()));
    const int p = sparse_[x];
    return 0 <= p && p < static_cast<int>(dense_.size()) && dense_[p] == x;
  }
  bool insert(int x) {
    if (contains(x)) return false;
    sparse_[x] = static_cast<int>(dense_.size()); dense_.push_back(x); return true;
  }
  bool erase(int x) {
    if (!contains(x)) return false;
    const int p = sparse_[x], y = dense_.back();
    dense_[p] = y; sparse_[y] = p; dense_.pop_back(); sparse_[x] = -1; return true;
  }
  void clear() { dense_.clear(); }
  int size() const { return static_cast<int>(dense_.size()); }
  const std::vector<int>& values() const { return dense_; }
};

}  // namespace ht

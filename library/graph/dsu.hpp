#pragma once

#include <cassert>
#include <numeric>
#include <utility>
#include <vector>

namespace ht {

class DSU {
  std::vector<int> parent_or_size_;
 public:
  explicit DSU(int n = 0) : parent_or_size_(n, -1) {}
  int leader(int x) { assert(0 <= x && x < static_cast<int>(parent_or_size_.size())); return parent_or_size_[x] < 0 ? x : parent_or_size_[x] = leader(parent_or_size_[x]); }
  bool merge(int a, int b) { a = leader(a); b = leader(b); if (a == b) return false; if (-parent_or_size_[a] < -parent_or_size_[b]) std::swap(a, b); parent_or_size_[a] += parent_or_size_[b]; parent_or_size_[b] = a; return true; }
  bool same(int a, int b) { return leader(a) == leader(b); }
  int size(int x) { return -parent_or_size_[leader(x)]; }
};

class RollbackDSU {
  std::vector<int> parent_or_size_;
  struct Change { int a, old_a, b, old_b; };
  std::vector<Change> history_;
 public:
  using Checkpoint = std::size_t;
  explicit RollbackDSU(int n = 0) : parent_or_size_(n, -1) {}
  int leader(int x) const { while (parent_or_size_[x] >= 0) x = parent_or_size_[x]; return x; }
  Checkpoint checkpoint() const { return history_.size(); }
  bool merge(int a, int b) { a = leader(a); b = leader(b); if (a == b) return false; if (-parent_or_size_[a] < -parent_or_size_[b]) std::swap(a, b); history_.push_back({a,parent_or_size_[a],b,parent_or_size_[b]}); parent_or_size_[a] += parent_or_size_[b]; parent_or_size_[b] = a; return true; }
  void rollback(Checkpoint cp) { while (history_.size() > cp) { auto c = history_.back(); history_.pop_back(); parent_or_size_[c.a] = c.old_a; parent_or_size_[c.b] = c.old_b; } }
  bool same(int a, int b) const { return leader(a) == leader(b); }
};

}  // namespace ht

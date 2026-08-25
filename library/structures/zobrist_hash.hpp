#pragma once

#include <cassert>
#include <cstdint>
#include <vector>
#include "../random/rng.hpp"

namespace ht {

class ZobristTable {
  int values_;
  std::vector<std::uint64_t> table_;

 public:
  ZobristTable(int positions, int values, std::uint64_t seed = 1)
      : values_(values), table_(static_cast<std::size_t>(positions) * values) {
    Xoshiro256StarStar rng(seed);
    for (auto& x : table_) x = rng();
  }
  std::uint64_t at(int position, int value) const {
    assert(position >= 0 && value >= 0 && value < values_);
    return table_[static_cast<std::size_t>(position) * values_ + value];
  }
  void change(std::uint64_t& hash, int position, int old_value, int new_value) const {
    hash ^= at(position, old_value) ^ at(position, new_value);
  }
};

}  // namespace ht

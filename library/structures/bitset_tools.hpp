#pragma once

#include <bit>
#include <cstdint>
#include <vector>

namespace ht {

inline int popcount(const std::vector<std::uint64_t>& bits) {
  int total = 0;
  for (auto x : bits) total += std::popcount(x);
  return total;
}
inline int intersection_count(const std::vector<std::uint64_t>& a,
                              const std::vector<std::uint64_t>& b) {
  int total = 0;
  for (std::size_t i = 0; i < a.size(); ++i) total += std::popcount(a[i] & b[i]);
  return total;
}

}  // namespace ht

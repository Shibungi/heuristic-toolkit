#pragma once

#include <array>
#include <cassert>

namespace ht {

struct GridIndex {
  int h, w;
  int id(int r, int c) const { assert(0 <= r && r < h && 0 <= c && c < w); return r * w + c; }
  int row(int id) const { return id / w; }
  int col(int id) const { return id % w; }
  bool inside(int r, int c) const { return 0 <= r && r < h && 0 <= c && c < w; }
  static constexpr std::array<int, 4> dr{-1, 0, 1, 0};
  static constexpr std::array<int, 4> dc{0, 1, 0, -1};
};

}  // namespace ht

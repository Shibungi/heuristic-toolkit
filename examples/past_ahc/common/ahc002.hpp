#pragma once

#include <array>
#include <iostream>
#include <string>

namespace past_ahc::ahc002 {

constexpr int N = 50;
constexpr std::array<int, 4> DR{-1, 0, 1, 0};
constexpr std::array<int, 4> DC{0, 1, 0, -1};
constexpr std::array<char, 4> DIR{'U', 'R', 'D', 'L'};

struct Input {
  int sr = 0, sc = 0;
  std::array<std::array<int, N>, N> tile{};
  std::array<std::array<int, N>, N> point{};
};

inline Input read() {
  Input in;
  std::cin >> in.sr >> in.sc;
  for (auto& row : in.tile) for (int& x : row) std::cin >> x;
  for (auto& row : in.point) for (int& x : row) std::cin >> x;
  return in;
}

inline bool inside(int r, int c) { return 0 <= r && r < N && 0 <= c && c < N; }
inline void print(const std::string& path) { std::cout << path << '\n'; }

}  // namespace past_ahc::ahc002

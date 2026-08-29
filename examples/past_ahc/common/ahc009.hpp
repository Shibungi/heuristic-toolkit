#pragma once

#include <array>
#include <iostream>
#include <string>

namespace past_ahc::ahc009 {

constexpr int H = 20, W = 20;
constexpr std::array<int, 4> DR{-1, 0, 1, 0};
constexpr std::array<int, 4> DC{0, 1, 0, -1};
constexpr std::array<char, 4> DIR{'U', 'R', 'D', 'L'};

struct Input {
  int sr = 0, sc = 0, tr = 0, tc = 0;
  double p = 0;
  std::array<std::string, H> horizontal{};  // 左右に隣接するマス間の壁
  std::array<std::string, H - 1> vertical{}; // 上下に隣接するマス間の壁
};

inline Input read() {
  Input in;
  std::cin >> in.sr >> in.sc >> in.tr >> in.tc >> in.p;
  for (auto& row : in.horizontal) std::cin >> row;
  for (auto& row : in.vertical) std::cin >> row;
  return in;
}

inline bool inside(int r, int c) { return 0 <= r && r < H && 0 <= c && c < W; }

inline bool can_move(const Input& in, int r, int c, int d) {
  const int nr = r + DR[d], nc = c + DC[d];
  if (!inside(nr, nc)) return false;
  if (d == 1) return in.horizontal[r][c] == '0';
  if (d == 3) return in.horizontal[r][c - 1] == '0';
  if (d == 2) return in.vertical[r][c] == '0';
  return in.vertical[r - 1][c] == '0';
}

inline std::pair<int, int> moved(const Input& in, int r, int c, int d) {
  if (!can_move(in, r, c, d)) return {r, c};
  return {r + DR[d], c + DC[d]};
}

inline int manhattan(const Input& in, int r, int c) {
  return std::abs(r - in.tr) + std::abs(c - in.tc);
}

// AHC009の出力は常に200文字。壁へ向かう指示も形式上は有効で、その場に留まる。
inline void print_200(std::string path) {
  path.resize(200, 'U');
  if (path.size() > 200) path.resize(200);
  std::cout << path << '\n';
}

}  // namespace past_ahc::ahc009

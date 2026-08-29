#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace past_ahc::ahc004 {

struct Input { int n = 0, m = 0; std::vector<std::string> words; };
using Grid = std::vector<std::string>;

inline Input read() {
  Input in;
  std::cin >> in.n >> in.m;
  in.words.resize(in.m);
  for (auto& s : in.words) std::cin >> s;
  return in;
}

// AHC004では盤面はトーラスとして扱われる。右・下の二方向を調べれば、
// 反対向きは開始位置をずらした同じ列挙で扱える。
inline bool contains(const Grid& g, const std::string& word) {
  const int n = static_cast<int>(g.size());
  for (int r = 0; r < n; ++r) for (int c = 0; c < n; ++c) {
    for (auto [dr, dc] : {std::pair{0, 1}, std::pair{1, 0}}) {
      bool ok = true;
      for (int k = 0; k < static_cast<int>(word.size()); ++k)
        ok &= g[(r + dr * k) % n][(c + dc * k) % n] == word[k];
      if (ok) return true;
    }
  }
  return false;
}

inline int score(const Input& in, const Grid& g) {
  int covered = 0;
  for (const auto& word : in.words) covered += contains(g, word);
  return covered;
}

inline std::vector<std::uint64_t> coverage_bits(const Input& in, const Grid& g) {
  std::vector<std::uint64_t> bits((in.m + 63) / 64);
  for (int i = 0; i < in.m; ++i)
    if (contains(g, in.words[i])) bits[i >> 6] |= 1ULL << (i & 63);
  return bits;
}

inline void print(const Grid& g) { for (const auto& row : g) std::cout << row << '\n'; }

}  // namespace past_ahc::ahc004

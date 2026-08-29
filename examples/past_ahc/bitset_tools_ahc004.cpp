#include <bits/stdc++.h>
#include "library/structures/bitset_tools.hpp"
#include "examples/past_ahc/common/ahc004.hpp"

using namespace std;
using namespace past_ahc::ahc004;

// bitset_toolsの使用例: AHC004で「どの文字列を含めたか」を64個ずつまとめる。
// 盤面候補を大量比較するとき、bool配列を毎回数えるよりAND+popcountが扱いやすい。
int main() {
  ios::sync_with_stdio(false); cin.tie(nullptr);
  const Input in = read();
  Grid grid(in.n, string(in.n, 'A'));

  // まず入力文字列を行へ順番に上書きするだけの構築解を作る。
  for (int i = 0; i < in.m; ++i) {
    const int r = i % in.n, start = (i / in.n) % in.n;
    for (int k = 0; k < static_cast<int>(in.words[i].size()); ++k)
      grid[r][(start + k) % in.n] = in.words[i][k];
  }

  const auto covered = coverage_bits(in, grid);
  const int covered_count = ht::popcount(covered);
  // 上位半分の重要語だけを表すmaskとの共通数も同じ関数で取れる。
  vector<uint64_t> first_half(covered.size());
  for (int i = 0; i < in.m / 2; ++i) first_half[i >> 6] |= 1ULL << (i & 63);
  const int important = ht::intersection_count(covered, first_half);
  cerr << "covered=" << covered_count << " first_half=" << important << '\n';
  print(grid);
}

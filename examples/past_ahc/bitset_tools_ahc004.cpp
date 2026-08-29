#include <bits/stdc++.h>
#include "library/structures/bitset_tools.hpp"
#include "examples/past_ahc/common/ahc004.hpp"

using namespace std;
using namespace past_ahc::ahc004;

// 【問題】AHC004: N×Nトーラス文字盤に、入力文字列をできるだけ多く含める最大化問題。
// 【この解法】入力語を行へ順に上書きする構築baseline。covered_countが大きいほど良い。
// 【State】Gridと、各入力語が盤面に存在するかを表すM-bit集合。
// 【bitset_toolsの役割】coverage_bitsのbit数と、重要語maskとの共通bit数を64-bit単位で数える。
// 【読む順】A埋め盤面→単語配置→全語の包含判定→bit集合→popcount→盤面出力。
// 【限界】後の単語が前の単語を壊す。探索はせず、集合評価の表現だけを示す。
// 公式問題: https://atcoder.jp/contests/ahc004/tasks/ahc004_a
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

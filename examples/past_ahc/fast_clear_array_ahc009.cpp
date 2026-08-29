#include <bits/stdc++.h>
#include "library/structures/fast_clear_array.hpp"
#include "examples/past_ahc/common/ahc009.hpp"

using namespace std;
using namespace past_ahc::ahc009;

// 【問題】AHC009: 指示が確率pで無視される迷路で、200手以内のゴール到達を目指す最大化問題。
// 【この解法】確率pを無視し、ゴールから決定論的BFSした距離が毎手減る方向を出力するbaseline。
// 【State】400マスのdist、BFS queue、現在座標、出力path。
// 【FastClearArrayの役割】dist全体をINFで埋めず、世代stampを進めてO(1)で論理初期化する。
// 【比較方向】距離は小さいほど良い。隣接マスのdistが最小の方向を選ぶ。
// 【限界】移動失敗確率を評価しない。確率分布Stateへ発展させる前の距離baseline。
// 公式問題: https://atcoder.jp/contests/ahc009/tasks/ahc009_a
// FastClearArrayの使用例: AHC009でゴールからBFSし、距離配列をO(1)で初期化する。
// 実戦では壁の推定値を更新するたびにBFSをやり直すため、400要素より大きい状態でも効く。
int main() {
  ios::sync_with_stdio(false); cin.tie(nullptr);
  const Input in = read();
  constexpr int INF = 1 << 20;
  ht::FastClearArray<int> dist(H * W, INF);
  dist.clear();  // 全要素を埋めず、世代番号だけを進める。
  queue<pair<int, int>> q;
  dist.set(in.tr * W + in.tc, 0); q.push({in.tr, in.tc});
  while (!q.empty()) {
    auto [r, c] = q.front(); q.pop();
    for (int d = 0; d < 4; ++d) {
      if (!can_move(in, r, c, d)) continue;
      auto [nr, nc] = moved(in, r, c, d); int id = nr * W + nc;
      if (dist.contains(id)) continue;
      dist.set(id, dist.get(r * W + c) + 1); q.push({nr, nc});
    }
  }
  int r = in.sr, c = in.sc; string path;
  while ((r != in.tr || c != in.tc) && path.size() < 200) {
    int chosen = -1;
    for (int d = 0; d < 4; ++d) if (can_move(in, r, c, d)) {
      auto [nr, nc] = moved(in, r, c, d);
      if (chosen < 0 || dist.get(nr * W + nc) < dist.get((r + DR[chosen]) * W + c + DC[chosen])) chosen = d;
    }
    if (chosen < 0) break;
    tie(r, c) = moved(in, r, c, chosen); path += DIR[chosen];
  }
  print_200(path);
}

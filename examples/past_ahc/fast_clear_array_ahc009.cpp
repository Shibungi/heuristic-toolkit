#include <bits/stdc++.h>
#include "library/structures/fast_clear_array.hpp"
#include "examples/past_ahc/common/ahc009.hpp"

using namespace std;
using namespace past_ahc::ahc009;

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

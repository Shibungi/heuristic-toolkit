#include <bits/stdc++.h>
#include "library/graph/shortest_path.hpp"
#include "examples/past_ahc/common/ahc009.hpp"

using namespace std;
using namespace past_ahc::ahc009;

// 【問題】AHC009: 確率的に移動指示が失敗する迷路で、到達時刻を早める。
// 【この解法】各マスを頂点、通路を無向辺にし、ゴールをsourceとしたBFS距離を計算する。
// 【目的】BFS距離の最小化。現在マスからdistが最小の隣接マスへ進む文字列を作る。
// 【shortest_pathの役割】graphとsourceから全頂点distだけを返す。経路復元は問題側のgreedyで行う。
// 【読む順】400頂点graph構築→ht::bfs→startから距離降下→200文字へpadding。
// 【限界】確率pを無視するため、同じ手を複数回送るなどのrobust性はない。
// 公式問題: https://atcoder.jp/contests/ahc009/tasks/ahc009_a
// shortest_pathの使用例: AHC009の迷路をグラフ化し、ゴールからBFSする。
// 失敗確率pを無視した決定論的baselineだが、改善案の比較対象として必ず残しておく。
int main() {
  ios::sync_with_stdio(false); cin.tie(nullptr);
  const Input in = read();
  vector<vector<int>> graph(H * W);
  for (int r = 0; r < H; ++r) for (int c = 0; c < W; ++c)
    for (int d = 0; d < 4; ++d) if (can_move(in, r, c, d)) {
      auto [nr, nc] = moved(in, r, c, d);
      graph[r * W + c].push_back(nr * W + nc);
    }
  const auto dist = ht::bfs(graph, {in.tr * W + in.tc});
  int r = in.sr, c = in.sc; string path;
  while ((r != in.tr || c != in.tc) && path.size() < 200) {
    int chosen = -1;
    for (int d = 0; d < 4; ++d) if (can_move(in, r, c, d)) {
      auto [nr, nc] = moved(in, r, c, d);
      if (chosen < 0 || dist[nr * W + nc] < dist[(r + DR[chosen]) * W + c + DC[chosen]]) chosen = d;
    }
    if (chosen < 0) break;
    tie(r, c) = moved(in, r, c, chosen); path += DIR[chosen];
  }
  print_200(path);
}

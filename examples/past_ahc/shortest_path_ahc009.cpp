#include <bits/stdc++.h>
#include "library/graph/shortest_path.hpp"
#include "examples/past_ahc/common/ahc009.hpp"

using namespace std;
using namespace past_ahc::ahc009;

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

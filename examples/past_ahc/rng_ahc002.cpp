#include <bits/stdc++.h>
#include "library/random/rng.hpp"
#include "examples/past_ahc/common/ahc002.hpp"

using namespace std;
using namespace past_ahc::ahc002;

// RNGの使用例: AHC002で同一seedなら完全に同じ経路を再現できるランダム化貪欲。
// スコア比較時に乱数列を固定できることが、近傍や評価関数のA/B比較では重要になる。
int main() {
  ios::sync_with_stdio(false); cin.tie(nullptr);
  const Input in = read();
  ht::Xoshiro256StarStar rng(20210822);
  string best; long long best_score = -1;
  for (int restart = 0; restart < 3000; ++restart) {
    vector<char> used(2500);
    int r = in.sr, c = in.sc; used[in.tile[r][c]] = true;
    string path; long long score = in.point[r][c];
    while (true) {
      array<int, 4> dirs{0, 1, 2, 3};
      // uniform_int()を使ったFisher-Yates。std::shuffleの実装差に依存しない。
      for (int i = 3; i > 0; --i) swap(dirs[i], dirs[rng.uniform_int(0, i)]);
      int chosen = -1;
      for (int d : dirs) {
        int nr = r + DR[d], nc = c + DC[d];
        if (inside(nr, nc) && !used[in.tile[nr][nc]]) { chosen = d; break; }
      }
      if (chosen < 0) break;
      r += DR[chosen]; c += DC[chosen]; used[in.tile[r][c]] = true;
      score += in.point[r][c]; path += DIR[chosen];
    }
    if (score > best_score) best_score = score, best = path;
  }
  print(best);
}
